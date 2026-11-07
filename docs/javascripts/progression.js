// Suivi de progression : chaque case à cocher (listes de tâches) est
// mémorisée dans le stockage local du navigateur. La clé est le texte de
// l'élément, ce qui synchronise une même étape entre la page du chapitre
// et la page Progression.
(function () {
  var PREFIXE = "cours-reverse:";

  function cle(li) {
    return PREFIXE + li.textContent.replace(/\s+/g, " ").trim();
  }
  function lire(k) {
    try { return window.localStorage.getItem(k) === "1"; } catch (e) { return false; }
  }
  function ecrire(k, valeur) {
    try {
      if (valeur) window.localStorage.setItem(k, "1");
      else window.localStorage.removeItem(k);
    } catch (e) { /* stockage indisponible : la case fonctionne sans mémoire */ }
  }

  function barre(conteneur, faits, total) {
    var pct = total ? Math.round((100 * faits) / total) : 0;
    conteneur.innerHTML =
      '<div class="progression-texte">' + faits + " étape(s) sur " + total +
      " terminée(s) : " + pct + " %</div>" +
      '<div class="progression-fond"><div class="progression-barre" style="width:' + pct + '%"></div></div>';
  }

  function mettreAJour() {
    var cases = document.querySelectorAll(".task-list-item input[type=checkbox]");
    var faits = 0;
    cases.forEach(function (c) { if (c.checked) faits++; });
    document.querySelectorAll(".progression-globale").forEach(function (el) {
      barre(el, faits, cases.length);
    });
  }

  function initialiser() {
    var cases = document.querySelectorAll(".task-list-item input[type=checkbox]");
    cases.forEach(function (c) {
      var li = c.closest("li");
      if (!li || c.dataset.suivi) return;
      var k = cle(li);
      c.dataset.suivi = "1";
      c.disabled = false;
      c.checked = lire(k);
      c.addEventListener("change", function () {
        ecrire(k, c.checked);
        mettreAJour();
      });
      // un clic sur le texte de l'étape coche aussi la case
      li.classList.add("etape-suivie");
      li.addEventListener("click", function (e) {
        if (e.target.closest("label, a, input")) return;
        c.checked = !c.checked;
        c.dispatchEvent(new Event("change"));
      });
    });
    document.querySelectorAll(".reinitialiser-progression").forEach(function (b) {
      if (b.dataset.suivi) return;
      b.dataset.suivi = "1";
      b.addEventListener("click", function () {
        if (!window.confirm("Effacer toute la progression enregistrée dans ce navigateur ?")) return;
        try {
          Object.keys(window.localStorage).forEach(function (k) {
            if (k.indexOf(PREFIXE) === 0) window.localStorage.removeItem(k);
          });
        } catch (e) {}
        document.querySelectorAll(".task-list-item input[type=checkbox]").forEach(function (c) {
          c.checked = false;
        });
        mettreAJour();
      });
    });
    mettreAJour();
  }

  if (window.document$ && window.document$.subscribe) {
    window.document$.subscribe(initialiser);   // navigation instantanée de Material
  } else {
    document.addEventListener("DOMContentLoaded", initialiser);
  }
})();
