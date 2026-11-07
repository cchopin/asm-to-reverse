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

  // État d'un chapitre : "fait" (toutes les cases cochées), "encours"
  // (au moins une), ou rien. Mémorisé par chemin de page pour pouvoir
  // décorer le menu depuis n'importe quelle page.
  var CLE_PAGE = PREFIXE + "page:";

  function enregistrerEtatPage() {
    // On ne considère que les cases du contenu, pas celles du menu.
    var contenu = document.querySelector(".md-content");
    if (!contenu) return;
    // La page Progression agrège toutes les cases : ne pas l'enregistrer.
    if (document.querySelector(".progression-globale")) return;
    var cases = contenu.querySelectorAll(".task-list-item input[type=checkbox]");
    if (!cases.length) return;
    var faits = 0;
    cases.forEach(function (c) { if (c.checked) faits++; });
    var etat = faits === cases.length ? "fait" : (faits > 0 ? "encours" : "");
    try {
      var k = CLE_PAGE + window.location.pathname;
      if (etat) window.localStorage.setItem(k, etat);
      else window.localStorage.removeItem(k);
    } catch (e) {}
  }

  function decorerMenu() {
    var liens = document.querySelectorAll(".md-nav--primary .md-nav__link[href]");
    liens.forEach(function (a) {
      var ancien = a.querySelector(".etat-chapitre");
      if (ancien) ancien.remove();
      var etat = etatDe(a);
      if (!etat) return;
      var s = document.createElement("span");
      s.className = "etat-chapitre " + etat;
      s.setAttribute("aria-hidden", "true");
      s.textContent = etat === "fait" ? "✓" : "•"; // coche ou puce
      s.title = etat === "fait" ? "Chapitre terminé" : "Chapitre commencé";
      a.appendChild(s);
    });
  }

  function etatDe(a) {
    try { return window.localStorage.getItem(CLE_PAGE + a.pathname); } catch (e) { return null; }
  }

  // Compteur "chapitres terminés / total" sur chaque partie du menu
  function compterParties() {
    document.querySelectorAll(".md-nav--primary .md-nav__item--nested").forEach(function (item) {
      var lien = item.querySelector(":scope > .md-nav__link, :scope > label.md-nav__link, :scope > .md-nav__container > .md-nav__link");
      if (!lien) return;
      var ancien = lien.querySelector(".compte-partie");
      if (ancien) ancien.remove();
      var chapitres = item.querySelectorAll(":scope > nav > .md-nav__list > .md-nav__item > a.md-nav__link[href]");
      if (!chapitres.length) return;
      var faits = 0;
      chapitres.forEach(function (a) { if (etatDe(a) === "fait") faits++; });
      if (!faits) return;
      var s = document.createElement("span");
      s.className = "compte-partie" + (faits === chapitres.length ? " complete" : "");
      s.textContent = faits + "/" + chapitres.length;
      s.title = faits + " chapitre(s) terminé(s) sur " + chapitres.length;
      var icone = lien.querySelector(".md-nav__icon");
      if (icone) lien.insertBefore(s, icone); else lien.appendChild(s);
    });
  }

  // Accueil : état de chaque étape du parcours et bouton "Reprendre"
  function decorerParcours() {
    var etapes = document.querySelectorAll(".parcours [data-chapitre]");
    if (!etapes.length) return;
    var prochaine = null, commence = false;
    etapes.forEach(function (a) {
      var etat = etatDe(a);
      a.classList.remove("fait", "encours");
      if (etat) { a.classList.add(etat); commence = true; }
      if (!prochaine && etat !== "fait") prochaine = a;
    });
    document.querySelectorAll(".partie-carte").forEach(function (carte) {
      var liens = carte.querySelectorAll("[data-chapitre]");
      var faits = carte.querySelectorAll("[data-chapitre].fait").length;
      var compte = carte.querySelector("[data-compte]");
      if (compte) compte.textContent = faits + " / " + liens.length;
    });
    var bouton = document.querySelector(".accueil .md-button--primary");
    if (bouton && commence && prochaine) {
      bouton.href = prochaine.href;
      var nom = prochaine.querySelector(".etape__nom");
      bouton.textContent = "Reprendre : " + (nom ? nom.textContent.trim() : "chapitre suivant");
    }
    var stat = document.querySelector("[data-progression-globale]");
    if (stat) {
      var n = 0;
      try {
        Object.keys(window.localStorage).forEach(function (k) {
          if (k.indexOf(PREFIXE) === 0 && k.indexOf(CLE_PAGE) !== 0) n++;
        });
      } catch (e) {}
      stat.textContent = n ? n + " étape(s) validée(s) dans ce navigateur" : "";
    }
  }

  function mettreAJour() {
    var cases = document.querySelectorAll(".task-list-item input[type=checkbox]");
    var faits = 0;
    cases.forEach(function (c) { if (c.checked) faits++; });
    document.querySelectorAll(".progression-globale").forEach(function (el) {
      barre(el, faits, cases.length);
    });
    enregistrerEtatPage();
    decorerMenu();
    compterParties();
    decorerParcours();
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
    barreLecture();
  }

  // Barre d'avancement de la lecture, en haut de l'écran
  function barreLecture() {
    var b = document.querySelector(".barre-lecture");
    if (!b) {
      b = document.createElement("div");
      b.className = "barre-lecture";
      b.setAttribute("aria-hidden", "true");
      document.body.appendChild(b);
      var maj = function () {
        var h = document.documentElement.scrollHeight - window.innerHeight;
        b.style.width = (h > 0 ? Math.min(100, (100 * window.scrollY) / h) : 0) + "%";
      };
      window.addEventListener("scroll", maj, { passive: true });
      window.addEventListener("resize", maj);
      b.maj = maj;
    }
    b.maj();
  }

  if (window.document$ && window.document$.subscribe) {
    window.document$.subscribe(initialiser);   // navigation instantanée de Material
  } else {
    document.addEventListener("DOMContentLoaded", initialiser);
  }
})();
