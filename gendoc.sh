#!/bin/dash

mkdir -p target/gendoc

for doc in doc/*.sh; do
    source=$(basename $doc)
    target=target/gendoc/${source%.sh}.md
    $SHELL $doc > $target
done
