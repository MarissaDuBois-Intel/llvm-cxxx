// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

const vscode = require("vscode");
const marker = "// -*- mode: c+++ -*-";

function activate(context) {
  const originalLanguages = new Map();
  async function update(document) {
    if (document.isClosed || document.uri.scheme !== "file")
      return;
    const language = document.languageId;
    if (!["c", "cpp", "cxxx"].includes(language))
      return;
    const marked = document.lineAt(0).text.trim() === marker;
    const key = document.uri.toString();
    if (marked && language !== "cxxx") {
      originalLanguages.set(key, language);
      await vscode.languages.setTextDocumentLanguage(document, "cxxx");
    } else if (!marked && language === "cxxx" && originalLanguages.has(key)) {
      const original = originalLanguages.get(key);
      originalLanguages.delete(key);
      await vscode.languages.setTextDocumentLanguage(document, original);
    }
  }
  function refresh(document) {
    update(document).catch(error =>
      vscode.window.showErrorMessage(`C+++ highlighting: ${error.message}`));
  }
  context.subscriptions.push(
    vscode.workspace.onDidOpenTextDocument(refresh),
    vscode.workspace.onDidChangeTextDocument(event => refresh(event.document))
  );
  vscode.workspace.textDocuments.forEach(refresh);
}

module.exports = { activate };
