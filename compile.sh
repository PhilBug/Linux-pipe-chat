#!/usr/bin/env bash
gcc inf127244_k.c inf127244_h.h -Wall -o klient
echo "file 'klient' ready"
gcc inf127244_s.c inf127244_h.h -Wall -o serwer
echo "file 'serwer' ready"