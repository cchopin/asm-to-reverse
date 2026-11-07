// Sélecteur de thème de couleur dans l'en-tête. Le choix est mémorisé
// dans le navigateur ; le clair / sombre reste géré par le bouton du thème.
(function () {
  var CLE = "cours-reverse-theme";
  var THEMES = [
    { id: "kanagawa", nom: "Kanagawa", couleurs: ["#1f1f28", "#7e9cd8", "#e6c384"] },
    { id: "gruvbox", nom: "Gruvbox", couleurs: ["#282828", "#fabd2f", "#8ec07c"] },
    { id: "rosepine", nom: "Rosé Pine", couleurs: ["#191724", "#c4a7e7", "#ebbcba"] },
    { id: "nord", nom: "Nord", couleurs: ["#2e3440", "#88c0d0", "#a3be8c"] }
  ];
  var ICONE = '<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M12 3a9 9 0 0 0 0 18c.83 0 1.5-.67 1.5-1.5 0-.39-.15-.74-.39-1.01-.23-.26-.38-.61-.38-.99 0-.83.67-1.5 1.5-1.5H16c2.76 0 5-2.24 5-5 0-4.42-4.03-8-9-8m-5.5 9c-.83 0-1.5-.67-1.5-1.5S5.67 9 6.5 9 8 9.67 8 10.5 7.33 12 6.5 12m3-4C8.67 8 8 7.33 8 6.5S8.67 5 9.5 5s1.5.67 1.5 1.5S10.33 8 9.5 8m5 0c-.83 0-1.5-.67-1.5-1.5S13.67 5 14.5 5s1.5.67 1.5 1.5S15.33 8 14.5 8m3 4c-.83 0-1.5-.67-1.5-1.5S16.67 9 17.5 9s1.5.67 1.5 1.5-.67 1.5-1.5 1.5"/></svg>';

  function actuel() {
    return document.documentElement.getAttribute("data-theme") || "kanagawa";
  }
  function appliquer(id) {
    document.documentElement.setAttribute("data-theme", id);
    try { localStorage.setItem(CLE, id); } catch (e) {}
    document.querySelectorAll(".choix-theme__option").forEach(function (o) {
      o.setAttribute("aria-checked", o.dataset.theme === id ? "true" : "false");
    });
  }

  function installer() {
    if (document.querySelector(".choix-theme")) return;
    var palette = document.querySelector(".md-header__inner [data-md-component=palette]");
    if (!palette) return;
    var bloc = document.createElement("div");
    bloc.className = "md-header__option choix-theme";
    var html = '<button class="md-header__button md-icon choix-theme__bouton" type="button" ' +
      'title="Thème de couleur" aria-label="Thème de couleur" aria-haspopup="true" aria-expanded="false">' +
      ICONE + '</button><div class="choix-theme__menu" role="menu" hidden>';
    THEMES.forEach(function (t) {
      html += '<button type="button" role="menuitemradio" class="choix-theme__option" data-theme="' + t.id + '">' +
        '<span class="choix-theme__pastilles">' +
        t.couleurs.map(function (c) { return '<i style="background:' + c + '"></i>'; }).join("") +
        '</span>' + t.nom + '</button>';
    });
    bloc.innerHTML = html + "</div>";
    palette.parentNode.insertBefore(bloc, palette);

    var bouton = bloc.querySelector(".choix-theme__bouton");
    var menu = bloc.querySelector(".choix-theme__menu");
    function ouvrir(v) {
      menu.hidden = !v;
      bouton.setAttribute("aria-expanded", v ? "true" : "false");
    }
    bouton.addEventListener("click", function (e) { e.stopPropagation(); ouvrir(menu.hidden); });
    menu.addEventListener("click", function (e) {
      var o = e.target.closest(".choix-theme__option");
      if (!o) return;
      appliquer(o.dataset.theme);
      ouvrir(false);
    });
    document.addEventListener("click", function (e) { if (!bloc.contains(e.target)) ouvrir(false); });
    document.addEventListener("keydown", function (e) { if (e.key === "Escape") ouvrir(false); });
    appliquer(actuel());
  }

  if (window.document$ && window.document$.subscribe) window.document$.subscribe(installer);
  else document.addEventListener("DOMContentLoaded", installer);
})();
