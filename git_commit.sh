#!/bin/sh
read -p "Commit comment: " s
git add .
git commit -m "\"${s}\""
git push origin Stray
