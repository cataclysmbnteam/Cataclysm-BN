#!/usr/bin/env -S deno run --allow-read --allow-write

import { Command } from "@cliffy/command"
import * as v from "@valibot/valibot"
import { parseMany, recursivelyReadJSONObj } from "./utils.ts"

type TalkResponse = {
  /** Text representing the user's choice/action */
  text?: string
  /** Topic ID to transition to */
  topic?: string
  /** Recursive reference for success branch */
  success?: TalkResponse
  /** Recursive reference for failure branch */
  failure?: TalkResponse
}
const TalkResponse: v.GenericSchema<TalkResponse> = v.lazy(() =>
  v.object({
    text: v.optional(v.string()),
    topic: v.optional(v.string()),
    success: v.optional(TalkResponse),
    failure: v.optional(TalkResponse),
  })
)

const DynamicLine = v.union([
  v.string(),
  v.object({
    speaker: v.optional(v.string()),
    text: v.string(),
  }),
])
type DynamicLine = v.InferOutput<typeof DynamicLine>

const TalkTopic = v.object({
  type: v.literal("talk_topic"),
  id: v.union([v.string(), v.array(v.string())]),
  dynamic_line: v.optional(DynamicLine),
  responses: v.optional(v.array(TalkResponse)),
  repeat_responses: v.optional(
    v.union([
      v.array(v.object({ response: v.optional(TalkResponse) })),
      v.object({ response: v.optional(TalkResponse) }),
    ]),
  ),
})
type TalkTopic = v.InferOutput<typeof TalkTopic>

const Npc = v.object({
  type: v.literal("npc"),
  chat: v.optional(v.string()),
})
type Npc = v.InferOutput<typeof Npc>

const DialogueEntry = v.union([TalkTopic, Npc])
type DialogueEntry = v.InferOutput<typeof DialogueEntry>

interface DialogueGraph {
  // Map topic ID to its Mermaid label text (using <br/> for newlines, escaped quotes)
  nodes: Map<string, string>
  // Set of full Mermaid edge definitions "SourceID --> TargetID : "Label""
  edges: Set<string>
}

const escapeMermaidLabel = (text: string): string => {
  if (!text) return ""

  const escaped = text.trim()
    .replace(/\\/g, "\\\\")
    .replace(/"/g, '\\"')
    .replace(/\n/g, "<br/>")
  return escaped
}

function trimLabel(text: string, maxLength = 120): string {
  const trimmedText = text.trim()
  if (
    trimmedText.length <= maxLength && !trimmedText.includes("\\n") &&
    !trimmedText.includes("<br/>")
  ) {
    return trimmedText
  }

  const mermaidNewlinesText = trimmedText.replace(/\\n/g, "<br/>")

  const lines = mermaidNewlinesText.split("<br/>")

  const brokenLines: string[] = []
  lines.forEach((line) => {
    if (line.length <= 40) {
      brokenLines.push(line)
    } else {
      const words = line.split(" ")
      let currentLine = ""
      words.forEach((word) => {
        if (currentLine.length + word.length + (currentLine.length > 0 ? 1 : 0) > 40) {
          if (currentLine.length > 0) {
            brokenLines.push(currentLine)
          }
          currentLine = word
        } else {
          if (currentLine.length > 0) {
            currentLine += " "
          }
          currentLine += word
        }
      })

      if (currentLine.length > 0) {
        brokenLines.push(currentLine)
      }
    }
  })

  const multilined = brokenLines.join("<br/>")

  if (multilined.length > maxLength) {
    let truncateIndex = maxLength

    const lastBr = multilined.lastIndexOf("<br/>", maxLength - 1)
    const lastSpace = multilined.lastIndexOf(" ", maxLength - 1)

    if (lastBr !== -1 && lastBr > maxLength - 20) {
      truncateIndex = lastBr
    } else if (lastSpace !== -1 && lastSpace > maxLength - 20) {
      truncateIndex = lastSpace
    } else {
      truncateIndex = maxLength
    }

    if (truncateIndex > 4 && multilined.substring(truncateIndex - 5, truncateIndex) === "<br/>") {
      truncateIndex -= 5
    }

    let truncated = multilined.substring(0, truncateIndex)

    if (truncated.length < multilined.length) {
      truncated = truncated.trim()
      if (truncated.endsWith("<br/>")) {
        truncated = truncated.substring(0, truncated.length - 5)
      }
      truncated += "..."
    }
    return truncated
  }

  return multilined
}

type TargetInfo = {
  target: string
  actionText?: string
}
const findTargets = (response: TalkResponse, actionTextSource?: string): TargetInfo[] => {
  const targets: TargetInfo[] = []

  const currentActionText = response.text !== undefined ? response.text : actionTextSource

  if (response.topic && response.topic !== "TALK_NONE") {
    if (response.topic !== "TALK_NONE") {
      targets.push({ target: response.topic, actionText: currentActionText })
    }
  }

  if (response.success) {
    targets.push(...findTargets(response.success, currentActionText))
  }
  if (response.failure) {
    targets.push(...findTargets(response.failure, currentActionText))
  }

  const uniqueTargets = new Map<string, string | undefined>()
  targets.forEach((t) => {
    if (!uniqueTargets.has(t.target)) {
      uniqueTargets.set(t.target, t.actionText)
    }
  })

  return Array.from(uniqueTargets.entries()).map(([target, actionText]) => ({ target, actionText }))
}

function extractGraphElementsMermaid(dialogueEntries: DialogueEntry[]): DialogueGraph {
  const nodes = new Map<string, string>()
  const edges = new Set<string>()
  const CHOICE_NODE_MARKER = "<<choice>>"

  const addNode = (id: string, text?: DynamicLine) => {
    if (nodes.has(id) && nodes.get(id) === CHOICE_NODE_MARKER) return // Don't overwrite choice marker

    let labelText = id

    if (typeof text === "string" && text.trim()) {
      labelText = trimLabel(escapeMermaidLabel(text))
    } else if (typeof text === "object" && text?.text?.trim()) {
      labelText = trimLabel(escapeMermaidLabel(text.text))
    }

    nodes.set(id, labelText)
  }

  dialogueEntries
    .filter((entry): entry is TalkTopic => entry.type === "talk_topic")
    .forEach((topic) => {
      const sourceIds = Array.isArray(topic.id) ? topic.id : [topic.id]
      const topicText = topic.dynamic_line

      const combinedResponses: TalkResponse[] = [...(topic.responses ?? [])]
      const repeat = topic.repeat_responses
      if (Array.isArray(repeat)) {
        combinedResponses.push(
          ...repeat
            .map((item) => (item && typeof item === "object" ? item.response : undefined))
            .filter(Boolean) as TalkResponse[],
        )
      } else if (repeat && typeof repeat === "object" && repeat.response) {
        combinedResponses.push(repeat.response)
      }

      sourceIds.forEach((sourceId) => {
        addNode(sourceId, topicText)

        const sourceSpecificTargets: TargetInfo[] = []
        combinedResponses.forEach((response) => {
          const targets = findTargets(response, response.text)
          sourceSpecificTargets.push(...targets)
        })

        const uniqueTransitionsMap = new Map<string, TargetInfo>()
        sourceSpecificTargets.forEach((t) => {
          const key = `${t.target}::${t.actionText ?? ""}` // Key by target and action text
          if (!uniqueTransitionsMap.has(key)) {
            uniqueTransitionsMap.set(key, t)
          }
        })
        const uniqueTransitions = Array.from(uniqueTransitionsMap.values())

        if (uniqueTransitions.length > 1) {
          const choiceId = `${sourceId}_choice`
          nodes.set(choiceId, CHOICE_NODE_MARKER)
          edges.add(`"${sourceId}" --> "${choiceId}"`)

          uniqueTransitions.forEach((targetInfo) => {
            const targetId = targetInfo.target
            const actionText = targetInfo.actionText
            if (!nodes.has(targetId)) addNode(targetId) // Ensure target exists

            const escapedActionText = escapeMermaidLabel(actionText ?? "")
            const trimmedActionText = trimLabel(escapedActionText, 60)
            const edgeLabel = trimmedActionText ? ` : "${trimmedActionText}"` : ""

            edges.add(`"${choiceId}" --> "${targetId}"${edgeLabel}`)
          })
        } else if (uniqueTransitions.length === 1) {
          const targetInfo = uniqueTransitions[0]
          const targetId = targetInfo.target
          const actionText = targetInfo.actionText
          if (!nodes.has(targetId)) addNode(targetId) // Ensure target exists

          const escapedActionText = escapeMermaidLabel(actionText ?? "")
          const trimmedActionText = trimLabel(escapedActionText, 30)
          const edgeLabel = trimmedActionText ? ` : "${trimmedActionText}"` : ""

          edges.add(`"${sourceId}" --> "${targetId}"${edgeLabel}`)
        }
      })
    })

  return { nodes, edges }
}

function generateMermaidString(graph: DialogueGraph): string {
  const CHOICE_NODE_MARKER = "<<choice>>"
  let mermaidOutput = "```mermaid\nstateDiagram-v2\n"
  mermaidOutput += "direction TD\n\n"

  const allNodeIds = new Set([
    ...graph.nodes.keys(),
    ...Array.from(graph.edges).flatMap((edge) => {
      const sourceMatch = edge.match(/^"([^"]+)"/)
      const targetMatch = edge.match(/-->\s*"([^"]+)"/)
      const ids: string[] = []
      if (sourceMatch) ids.push(sourceMatch[1])
      if (targetMatch) ids.push(targetMatch[1])
      return ids
    }),
  ])

  allNodeIds.forEach((id) => {
    const nodeValue = graph.nodes.get(id)

    if (nodeValue === CHOICE_NODE_MARKER) {
      mermaidOutput += `  state "${id}" ${CHOICE_NODE_MARKER}\n`
    } else {
      const label = nodeValue ?? id
      const displayLabel = label.trim() === "" ? id : label

      mermaidOutput += `  "${id}" : ${escapeMermaidLabel(`${id}\n${displayLabel}`)}\n`
    }
  })
  mermaidOutput += "\n"

  graph.edges.forEach((edge) => {
    mermaidOutput += `  ${edge}\n`
  })

  return mermaidOutput + "```"
}

if (import.meta.main) {
  const { args: paths } = await new Command()
    .name("./scripts/dialogue_visualizer.ts")
    .version("0.0.0-rc.1")
    .description("Generate a Mermaid state diagram representing dialogue from given files")
    .arguments("<path...:string>")
    .usage("<path...> [options]")
    .example(
      "Process Dino Dave",
      "deno run -R ./scripts/dialogue_visualizer.ts data/json/npcs/refugee_center/surface_refugees/NPC_Dino_Dave.json > dave_dialogue.md",
    )
    .parse(Deno.args)

  console.error(`Reading and validating JSON data from: ${paths.join(", ")}...`)
  const jsons = (await recursivelyReadJSONObj(...paths)).flatMap((x) => x.data)
  const dialogueData: DialogueEntry[] = parseMany(DialogueEntry)(jsons)
  console.error(`Found ${dialogueData.length} valid dialogue entries.`)

  if (dialogueData.length === 0) {
    console.warn(
      "Warning: No valid dialogue entries found after validation. No Mermaid file generated.",
    )
    Deno.exit(0)
  }

  console.error("Generating dialogue graph elements for Mermaid...")
  const graph = extractGraphElementsMermaid(dialogueData)

  const allEdgeNodes = Array.from(graph.edges).flatMap((edge) => {
    const sourceMatch = edge.match(/^"([^"]+)"/)
    const targetMatch = edge.match(/-->\s*"([^"]+)"/)
    const ids: string[] = []
    if (sourceMatch) ids.push(sourceMatch[1])
    if (targetMatch) ids.push(targetMatch[1])
    return ids
  })
  new Set(allEdgeNodes).forEach((id) => {
    if (!graph.nodes.has(id)) {
      graph.nodes.set(id, id)
    }
  })

  if (graph.nodes.size === 0 && graph.edges.size === 0) {
    console.warn(
      "Warning: No talk topics or transitions found to generate a graph. No Mermaid file generated.",
    )
    Deno.exit(0)
  }

  console.error(`Extracted ${graph.nodes.size} nodes and ${graph.edges.size} edges.`)

  const mermaidOutput = generateMermaidString(graph)
  console.log(mermaidOutput)
}
