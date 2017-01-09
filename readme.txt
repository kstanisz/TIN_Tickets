Instalacja connectora MySQL:

sudo apt-get install libmysqlclient-dev

Do zrobienia:

* dodanie daty ważności do struktury biletu.
* usługa UDP TIME - zwracanie czasu po stronie serwera, odbiór czasu po stronie klienta
* kryptografia
* wiadomości nie zawsze poprawnie się przesyłają co skutkuje błędem przy deserializacji. Edit: UDP może gubić pakiety. Trzeba obsłużyć wyjątek gdy deserializacja jsona się nie powiedzie.
* baza danych biletów po stronie klienta (obecnie wszystko dzieje się w pamięci clienta)