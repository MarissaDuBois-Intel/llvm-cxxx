// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

const assert = require("node:assert/strict");
const fs = require("node:fs");
const path = require("node:path");
const vscode = require("vscode");

async function waitForLanguage(document, language) {
  for (let attempt = 0; attempt < 100; ++attempt) {
    const current = await vscode.workspace.openTextDocument(document.uri);
    if (current.languageId === language)
      return current;
    await new Promise(resolve => setTimeout(resolve, 50));
  }
  throw new Error(`Document did not switch to ${language}`);
}

async function run() {
  await require("./grammar-tests.cjs").run(vscode.env.appRoot);
  const extension = vscode.extensions.getExtension("cxxx-local.cxxx-syntax");
  assert(extension, "C+++ extension is not registered");
  await extension.activate();
  const folder = process.env.CXXX_EDITOR_TEST_DIR;
  assert(folder, "CXXX_EDITOR_TEST_DIR is required");
  fs.mkdirSync(folder, { recursive: true });
  const markedFile = path.join(folder, "marked.cpp");
  const ordinaryFile = path.join(folder, "ordinary.cpp");
  fs.writeFileSync(markedFile, "// -*- mode: c+++ -*-\ntrine state = other;\n");
  fs.writeFileSync(ordinaryFile, "int other = 0;\n");
  try {
    let marked = await vscode.workspace.openTextDocument(markedFile);
    marked = await waitForLanguage(marked, "cxxx");
    assert.equal(vscode.workspace.getConfiguration("editor", marked)
      .get("semanticHighlighting.enabled"), false);
    const ordinary = await vscode.workspace.openTextDocument(ordinaryFile);
    assert.equal(ordinary.languageId, "cpp");
    const edit = new vscode.WorkspaceEdit();
    edit.replace(marked.uri, marked.lineAt(0).range, "// Ordinary C++");
    assert(await vscode.workspace.applyEdit(edit));
    marked = await waitForLanguage(marked, "cpp");
    await marked.save();
    console.log("PASS: VS Code extension activation, marker detection and ordinary C++ isolation");
  } finally {
    fs.unlinkSync(markedFile);
    fs.unlinkSync(ordinaryFile);
  }
}

module.exports = { run };
