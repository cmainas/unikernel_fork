# Επικοινωνία μέσω sockets (rumprun)

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών που τρέχουν σε rumprun και επικοινωνούν μέσω sockets. Στον client δίνουμε ως όρισμα την IP του server

Για δοκιμή, αφού πρώτα γίνει build το rumprun, ακολουθώντας τις οδηγίες [ςδώ](https://github.com/rumpkernel/wiki/wiki/Tutorial%3A-Building-Rumprun-Unikernels), στη συνέχεια χρησιμοποιώντας τον cross compiler που δίνει το rumprun και την εντολή rumprun-bake φτιάχνουμε τα bins. Μπορεί να χρησιμοποιηθεί και το Makefile σε αυτό το φάκελο, αλλάζοντας στις 2 πρώτες γραμμές τη διεύθυνση που έχει εγκατασταθεί το κάθε βοηθητικό rumprun πρόγραμμα. 

Τέλος, ξεκινάμε τους unikernels χρησιμοποιώντας τις εντολές
```sh
$ rumprun kvm -i -I if,vioif,'-net bridge,br=br0' -W if,inet,static,IP_addr/mask server-rumprun.bin
$ rumprun kvm -i -I if,vioif,'-net bridge,br=br0' -W if,inet,static,IP_addr/mask client-rumprun.bin SERVER_IP_OR_HOSTNAME
```
αλλάζοντας τα πεδία IP_addr/mask αναλόγως.

Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.

