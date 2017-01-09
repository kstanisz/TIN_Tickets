Instalacja connectora MySQL:

sudo apt-get install libmysqlclient-dev

Do zrobienia:

* dodanie daty ważności do struktury biletu.
* usługa UDP TIME - zwracanie czasu po stronie serwera, odbiór czasu po stronie klienta
* kryptografia
* wiadomości nie zawsze poprawnie się przesyłają co skutkuje błędem przy deserializacji.
* baza danych biletów po stronie klienta (obecnie wszystko dzieje się w pamięci clienta)