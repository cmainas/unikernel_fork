Pipe μέσω shared memory (rumprun part 3)

Σε αυτό το φάκελο υπάρχουν οι κώδικες των εφαρμογών, του driver και διάφορα άλλα
απαραίτητα αρχεία για το χτίσιμο του rumprun. 

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
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i client-rumprun.bin
$ rumprun kvm -g "-vga none -nographic -device ivshmem-plain,memdev=hostmem -object memory-backend-file,size=1M,share,mem-path=/dev/shm/ivshmem,id=hostmem" -i server-rumprun.bin
```
Στο φάκελο test, υπάρχει ένα απλό παράδειγμα όπου ο server ανταλλάσει μηνύματα 
με τον client.
Στο φάκελο test1, υπάρχει ένα παράδειγμα όπου 2 clients γράφουν πολλά πράγματα
στο pipe και ο server τα διαβάζει. 
Στο φάκελο test2, υπάρχει ένα παράδειγμα όπου 1 client γράφει πολλά πράγματα
στο pipe (πάνω από το pipe size) και ο server τα διαβάζει. Επίσης ελέγχεται 
αν στην write χωρίς readers επιστρέφεται EPIPE.
Αν όλα έχουν πάει καλά πρέπει να έχουν σταματήσει και οι 2 unikernels μετά από λίγο.


