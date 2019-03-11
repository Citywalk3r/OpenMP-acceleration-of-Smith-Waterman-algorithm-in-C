#!/bin/sh
set username StrayLamb2
set pass Secretcode2!
read -p "Commit comment: " s
git add .
git commit -m "\"${s}\""
git push origin Stray

expect "*"
send "${username}\r"

expect "*"
send "${pass}\r"

expect eof
