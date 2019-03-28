#!/bin/sh
read -p "Commit comment: " s
read -p "Enter branch: " b
git add .
git commit -m "${s}"
git push origin HEAD:"${b}"
