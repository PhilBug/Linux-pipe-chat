# Linux pipe chat
Program należy skompilować za pomocą skyptu compile.sh, a następnie uruchomić 
serwer komendą ./serwer i dowolną ilość klientów za pomocą ./klient.

W pliku inf127244_k.c znajduje się program klienta, ma on minimalną funkcjonalność,
potrafi jedynie wysyłać wiadomości do serwera i otrzymywać od serwera wiadomości zwrotne,
odczytywać wiadomości od innych użytkowników oraz zamknąć swój własny program.

Plik inf127244_s.c zawiera program serwera, który przetwarza wszystkie komunikaty klienta
i wykonuje odpowiadające im polecenia. W nim znajdują się wszystkie istotne komponenty.
