// hook_strcmp.js : avec Frida, journalise chaque appel a strcmp et
// force le resultat a 0 (egalite). Usage (sur votre machine) :
//   frida -l hook_strcmp.js ./motdepasse essai
Interceptor.attach(Module.getExportByName(null, 'strcmp'), {
    onEnter(args) {
        this.a = args[0].readUtf8String();
        this.b = args[1].readUtf8String();
        console.log('strcmp("' + this.a + '", "' + this.b + '")');
    },
    onLeave(retval) {
        // Observer ce que ltrace montrerait : les deux chaines comparees.
        // Pour CONTOURNER le test, forcer l'egalite :
        retval.replace(0);
    }
});
