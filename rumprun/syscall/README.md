Pipe μέσω sockets 

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών, του system call και 
διάφορα άλλα 
απαραίτητα αρχεία για το χτίσιμο του rumprun. O server είναι ρυθμισμένος να
ακούει στην IP 192.168.1.13 και ο client να στέλνει το μήνυμα σε αυτήν. Ωστόσο,
αυτό μπορεί να αλλάξει από τον φάκελο dev_comso.h και στη σταθερά SERVER_IP. 

O ορισμός του system call είναι int my_pipe(int *fildes).

Για δοκιμή, αφού κάνουμε clone το repo του rumprun και τα submodules, αλλάζουμε
στο script pre_build.sh το path του rumprun repo και εκτελούμε
το script pre_build.sh που ελέγχει αρχικά αν η libunwind έχει γίνει patched και
αν όχι τότε το κάνει αυτό χρησιμποιοώντας το as.patch. Έπειτα μεταφέρει τα
αρχεία στους κατάλληλους φακέλους. Τέλος χτίζουμε το rumprun με τo script
build-rr.sh hw. 

Αφού τελειώσει το χτίσιμο, φτιάχνουμε τα bins του rumprun, για αυτό το σκοπό
μπορεί να χρησιμοποιηθεί και το Makefile που υπάρχει στον παρών φάκελο,
αλλάζοντας κατάλληλα στις 2 πρώτες γραμμές το path που έχει εγκατασταθεί το κάθε
βοηθητικό rumprun πρόγραμμα. 

Τέλος, ξεκινάμε τους unikernels χρησιμοποιώντας τις εντολές
```sh
$ rumprun kvm -i -I if,vioif,'-net bridge,br=br0' -W if,inet,static,IP_addr/mask server-rumprun.bin
$ rumprun kvm -i -I if,vioif,'-net bridge,br=br0' -W if,inet,static,IP_addr/mask client-rumprun.bin 
```
αλλάζοντας τα πεδία IP_addr/mask αναλόγως.

Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.


