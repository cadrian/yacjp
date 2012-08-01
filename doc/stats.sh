#!/bin/dash

catcount() {
    echo -n "| $1:"
    emph="$2"
    shift 2
    echo -n ' | '"$emph"$(cat "$@" | wc -l)' lines'"$emph"
    echo -n ' | '"$emph"$(cat "$@" | egrep -v '^ *(/?(\*|/)|$)' | wc -l)' lines'"$emph"
    echo ' |'
}

count() {
    echo '| Name | With comments | Without comments |'
    echo '| ----: | ----: | ----: |'
    catcount '_Total_' '_' "$@"
    for f in "$@"; do
        catcount '`'"$f"'`' '' "$f"
    done
    echo
}

#echo '\defgroup stats Statistics'
echo '## Statistics'
echo

echo '### Public headers'
(
    cd include; count *.h
)

echo '### Source files'
(
    cd src; count *.c
)

echo '### Test files'
(
    cd test; count *.[ch]
)
