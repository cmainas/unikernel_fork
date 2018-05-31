Επικοινωνία μέσω sockets (osv)

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών που τρέχουν σε osv και
επικοινωνούν μέσω sockets. Στον client δίνουμε ως όρισμα την IP του server 

Για δοκιμή, αφού κάνουμε clone το osv και ακολουθήσουμε τις οδηγίες στο
https://github.com/cloudius-systems/osv, πριν κάνουμε make
προσθέτουμε στο usr.manifest.skel δύο νέες γραμμές, με τα 2 shared objects των
εφαρμογών (server.so, client.so). Για τοσκοπό αυτό μπορεί να χρησιμποιηθεί το
Makefile.

/server.so: /path/to/server.so
/client.so: /path/to/client.so

Στη συνέχεια, κάνουμε make και τρέχουμε το script build. Αντιγράφουμε το usr.img
στο /build/last ως usr1.img πχ στον ίδιο φάκελο. Ξεκινάμε 2 instances του osv με
τις παρκάτω εντολές.

./scripts/run.py --novnc --nogdb --mac 52:54:00:12:1a:2a -n -b br0 -V -m 500M -c 2 -e "server.so"
και
./scripts/run.py -i build/last/usr1.img --novnc --nogdb --mac 52:54:00:32:2a:1a
-n -b br0 -V -m 500M -c 2 -e "client.so SERVER_IP"

βάζοντας ως παράμετρο στο client.so την ip που έχει πάρει ο server.
