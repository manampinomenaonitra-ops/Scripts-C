#!/bin/bash
for i in $(seq 1 8); do
    (echo "Client $i : bonjour" | nc -q 1 127.0.0.1 9999) &
done
wait
echo 'Tous les clients ont terminé'
