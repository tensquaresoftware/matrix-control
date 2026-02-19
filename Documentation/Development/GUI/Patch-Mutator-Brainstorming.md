# Matrix-Control / Patch Mutator

Brainstorming avec Claude IA

Date: 2026-02-10

---

## Guillaume

Je développe un éditeur MIDI pour le synthé Oberheim Matrix-1000 avec JUCE 8. Je suis en train de réfléchir à l'ajout d'un module assez fun dans la GUI qui s'intitulera "Patch Mutator". Ce module s'inspire très largement du plugin Absynth 5 de Native Instruments avec son module "Sound Mutation".

Tu trouveras en pièce jointe un mockup de mon Patch Mutator, je te le décris :

- Les toggles D1, D2, F/A, F/T, R/P, E1, E2, E3, L1 et L2 permettent d'indiquer au Patch Mutator quels modules de synthèse et donc quels paramètres du synthé doivent être altérés par la mutation (il y a 10 modules : DCO 1, DCO 2, etc. et une centaine de paramètres en tout).
- Le slider Amount définit l’ampleur de la mutation en %. Plus on augmente le Amount, plus le son du patch muté s’éloignera du son du patch original.
- Le slider Random définit la quantité de randomisation en %, appliquée aux paramètres affectés par la mutation.
- Le bouton Mutate lance la mutation à partir du patch courant (qui peut déjà être un patch muté), ce dernier est alors remplacé en mémoire par la nouvelle mutation.
- Le bouton Retry annule la dernière mutation et en applique une nouvelle en partant du patch ou de la mutation précédent(e).
- La combobox History donne un accès aléatoire à l'ensemble des mutations et des retries effectués depuis le chargement d'un patch du synthé.
- Les mutations sont automatiquement nommées M1, M2, etc. et les retries sont automatiquement nommés sur la base de la mutation qui leur a donné naissance, comme par exemple : M3-R1, M3, R2, etc. pour des retries effectués à partir de la mutation M3.
- Il peut y avoir 9 mutations au maximum (M1 à M9), et 9 retries par mutation (R1 à R9).
- Le bouton Clear purge l'ensemble des mutations et des retries stockés en mémoire, ce qui réinitialise la liste de la combobox History.
- Les boutons < et > permettent de naviguer circulairement dans la liste de la combobox History.
- Le bouton Save Patches sauvegarde automatiquement toutes les mutations et retries de l'historique, vers des fichiers .syx individuels sur le disque dur de l'ordinateur. Cela permet à l'utilisateur de pouvoir recharger ces patches lors d'une session ultérieure dans son DAW, l'historique du Patch Mutator n'étant pas persistant.
- Je te passe les détails concernant la logique GUI : certains boutons deviennent inactifs selon certains scénarios, etc. Ce n'est pas le plus important pour le moment.

Je souhaite avoir ton avis sur mon Patch Mutator, et notamment que tu me conseilles ou me fasses remarquer si j'ai oublié des primitives importantes. Par exemple, on ne peut pas supprimer une mutation ou un retry de son choix dans l'historique, le bouton Clear supprime l'ensemble. Est-ce gênant ? Par contre on peut sauver une mutation ou un retry à la volée lorsqu'il est intéressant, grâce à un autre module de mon plugin qui s'intitule Computer Patches (il y a un bouton Save As).

Je suis assez limité en place disponible dans la GUI pour implémenter ce Patch Mutator. Chaque pixel compte et je dois bien réfléchir à mes widgets essentiels, et ceux qui peuvent être éventuellement remplacés par des widgets plus intéressants pour les fonctionnalités. Par exemple, les boutons < et > sont très pratiques pour naviguer dans l'historique, mais ils pourraient éventuellement ne pas exister car la combobox permet déjà cette navigation. La place récupérée dans la GUI pourrait alors servir à implémenter d'autres widgets avec d'autres fonctionnalités plus intéressantes (et peut-être vitales) auxquelles je n'ai pas encore pensées. C'est pour cette raison que je te sollicite principalement : m'aider à penser à des détails auxquels je n'ai pas encore pensés pour ce Patch Mutator.



## Claude IA

Ton Patch Mutator est bien pensé et s'inscrit clairement dans la logique d'Absynth 5 tout en l'adaptant au contexte MIDI/hardware du Matrix-1000. Voici mes observations, des plus critiques aux plus mineures.

------

**Le patch original n'est pas dans l'historique — c'est un oubli important**

C'est probablement la primitive la plus critique qui manque. Lorsque l'utilisateur charge un patch et commence à muter, il perd la référence au patch d'origine dès la première mutation, car l'historique ne semble commencer qu'à M1. Or, dans un workflow de mutation, revenir au point de départ et comparer est fondamental. Je suggèrerais d'ajouter une entrée fixe en tête de la combobox History — appelée `ORIG` ou `M0` — qui représente toujours le patch tel qu'il était avant toute mutation. Cette entrée ne consomme pas de slot de mutation, elle est juste le point d'ancrage.

------

**L'A/B compare est absent**

Très lié au point précédent : il n'existe aucun moyen de basculer rapidement entre le patch courant et le patch original (ou une autre référence) pour évaluer l'écart sonore. C'est pourtant le geste le plus naturel dans ce type de workflow. Un bouton `A/B` — ou même un double-clic sur l'entrée ORIG dans la combobox — permettrait cette bascule instantanée. Et ça tombe bien : ce bouton pourrait très bien remplacer l'un des deux boutons `<` et `>`, dont la valeur ajoutée est effectivement limitée par rapport à la combobox.

------

**Le comportement de Retry mérite d'être précisé — et peut-être reconsidéré**

Tel que tu le décris, Retry *supprime* la tentative précédente et en génère une nouvelle. Cela signifie que si tu es sur M3-R2 et que tu pressess Retry, M3-R2 disparaît au profit de M3-R3. C'est une perte d'information définitive potentiellement frustrante. L'utilisateur pourrait avoir envie de conserver M3-R2 tout en explorant M3-R3.

Une alternative serait de traiter Retry exactement comme Mutate mais depuis le parent de la sélection courante, et de ne pas supprimer la tentative précédente — ce qui revient simplement à empiler les retries sans en effacer. La limite de 9 retries par mutation s'en charge naturellement. Si tu gardes la sémantique actuelle (suppression), il faut au moins un warning ou une confirmation, sinon l'utilisateur risque de perdre une mutation intéressante par accident.

------

**La suppression unitaire — tu l'as soulevé toi-même, et c'est effectivement gênant**

L'absence de suppression individuelle d'une entrée dans l'historique est une vraie limitation, mais elle est partiellement compensée par le fait que Save As dans ton module Computer Patches permet de sauvegarder une entrée intéressante à la volée. La seule friction, c'est que l'utilisateur doit y penser *avant* de faire Clear. Un petit workflow UX à documenter clairement dans le plugin (tooltip ou label) pourrait suffire à mitiger ce problème sans ajouter de widget.

------

**L'envoi MIDI au synthé : automatique ou manuel ?**

C'est implicite dans ta description mais pas explicite : quand l'utilisateur navigue dans l'historique (via combobox ou < >), est-ce que le patch correspondant est automatiquement envoyé au Matrix-1000 pour être auditionné ? Ou faut-il une action explicite ? Si c'est automatique, c'est le bon choix mais il faut s'assurer que la navigation au clavier dans la combobox ne spamme pas le SysEx. Si c'est manuel, un bouton `SEND` ou `▶` devient une primitive nécessaire, et ça change ton layout.

------

**Le comportement aux limites (M9, R9)**

Que se passe-t-il quand l'utilisateur essaie de créer une 10ème mutation alors que M9 existe déjà ? Le bouton Mutate doit clairement devenir inactif (tu l'as probablement prévu), mais l'utilisateur doit comprendre *pourquoi*. Un label ou un indicateur visuel du compteur courant (`3/9` par exemple, à côté du label History) serait utile sans prendre beaucoup de place.

------

**Sur les boutons < et > : ma recommandation**

Je pense que tu peux les supprimer. La combobox suffit pour la navigation, et la place récupérée (assez significative) pourrait accueillir quelque chose comme un bouton `A/B` pour la comparaison, ou un indicateur visuel de progression dans les limites de slots. Les `< >` n'ajoutent véritablement de valeur que pour une navigation sans souris (clavier/MIDI controller), ce qui est un cas d'usage marginal dans un éditeur de patches.

------

En résumé, les deux primitives les plus importantes à ajouter selon moi sont l'entrée `ORIG` dans l'historique et un mécanisme d'A/B compare. Tout le reste est soit mineur, soit déjà couvert par d'autres modules de ton plugin.

---

## Guillaume

Concernant le comportement de Retry, c'est ainsi qu'il fonctionne dans Absynth 5. Si l'utilisateur en est à M3-R2 et qu'il clique sur Retry, cela ne supprime pas le M3-R2 dans l'historique (on pourra donc y revenir via la combobox) mais génère un M3-R3 placé immédiatement après M3-R2 dans la liste de la combobox. Par contre, pour générer M3-R3, on repart des valeurs des paramètres telles qu'elles étaient juste avant la génération de M3-R2. C'est ce dernier point qui est capital : le bouton Mutate altère systématiquement les paramètres concernés à chaque nouvelle mutation, alors que Retry revient systématiquement aux valeurs précédentes des paramètres avant de lancer une nouvelle mutation, et ce afin d'éviter des sauts trop violents dans les changements sonores.

Je te fais passer mon mockup mis à jour :

J'ai implémenté un nouveau toggle Compare, il est très courant sur les synthés numériques hardware. Le texte devient rouge (peut-être clignotant, je verrai si c'est faisable) lorsque le toggle est actif, signifiant alors qu'on écoute le patch initial. Dans cet état, la combobox est grisée, empêchant d'accéder à l'historique. En cliquant à nouveau sur le toggle, le texte redevient blanc (Normal) et on écoute à nouveau la mutation précédemment sélectionnée dans l'historique, la combobox étant à nouveau accessible.

Comme tu pourras le voir, j'ai ajouté des widgets et réordonné mes boutons différemment :

- Verticalement : on Mutate, puis classiquement on Retry, puis on Compare (le patch initialement chargé avant toute mutation est donc rechargeable via ce toggle).
- Horizontalement : on navigue dans l'historique via la combobox, puis on Compare avec le patch initial, puis on peut supprimer la dernière mutation ou retry de l'historique avec Del (Delete), puis on peut purger l'historique complet avec Clear, puis on Export l'historique complet en fichiers .syx de patches individuels (il y aura une boite de dialogue pour pouvoir indiquer un dossier de destination).

Concernant les envois au synthé, cela se fait immédiatement après avoir cliqué sur Mutate ou Retry, le reste de mon plugin fonctionne selon ce même principe temps réel. La seule subtilité avec le Patch Mutator, c'est que je dois envoyer un message SysEx complet de patch, au lieu d'un simple message SysEx de changement de valeur de paramètre.

Concernant le comportement aux limites, ma GUI est équipée d'une barre de messages tout en bas de la fenêtre qui me permettra d'afficher des messages et avertissements à l'utilisateur.

Concernant le bouton Save Patches renommé en Export, cela a l'avantage d'être plus court et de mieux indiquer l'idée que tout l'historique est exporté d'un coup en fichiers .syx de patches individuels. En effet, avec le nom Save Patches, on pourrait croire qu'on a la possibilité de choisir quels mutations de patches on peut sauver ici.

Que penses-tu de ces améliorations ? Vois-tu d'autres oublis vitaux ou importants ?

---

## Claude IA

Le redesign est solide. Le positionnement vertical MUTATE → RETRY → COMPARE a une logique workflow naturelle, et le COMPARE en toggle rouge (clignotant si tu y arrives, c'est un détail d'UX très agréable) est exactement ce qu'il fallait. Voici mes observations.

------

**La question la plus critique que ton design ne résout pas encore : Mutate depuis une entrée non-terminale**

C'est le vrai trou dans la raquette. L'utilisateur navigue dans l'historique via la combobox, se retrouve sur M3-R2 (alors que l'historique va jusqu'à M7-R4), écoute ce patch, le trouve intéressant, et clique sur MUTATE. Que se passe-t-il ?

Tu as trois options conceptuelles, chacune avec ses conséquences :

**Option A — Mutate crée toujours à partir de la tête de l'historique**, peu importe ce qui est sélectionné dans la combobox. Simple, prévisible, mais l'utilisateur qui pensait muter depuis ce qu'il entend sera surpris. MUTATE devrait alors se désactiver ou changer de comportement visible si la sélection n'est pas la tête.

**Option B — Mutate crée à partir de l'entrée sélectionnée et tronque l'historique** à partir de ce point. M4, M5, M6, M7 et leurs retries disparaissent, et M4 est créé depuis M3-R2. C'est destructif mais cohérent. Nécessite une confirmation ou un warning dans ta barre de messages.

**Option C — Mutate crée une vraie branche**, ce qui rompt ta nomenclature plate M1→M9 et complexifie l'ensemble. À éviter compte tenu de tes contraintes de GUI.

Je pencherais pour l'Option B avec un message dans ta barre du type *"Mutating from M3-R2 — subsequent history cleared."* Mais c'est toi qui connais ton utilisateur cible.

------

**Le comportement de DL mérite d'être précisé — surtout en cas de suppression avec descendants**

"Delete Last" est clair tant qu'on parle de la dernière entrée de la liste. Mais que se passe-t-il si tu supprimes M3 alors que M3-R1 à M3-R5 existent encore dans l'historique ? Soit tu supprimes en cascade (M3 + tous ses retries), ce qui est logique mais destructif, soit tu interdis la suppression tant que des retries existent. Dans les deux cas, un message dans ta barre d'état s'impose.

Par ailleurs, je suggérerais d'envisager que DL supprime toujours **la dernière entrée de la liste** (pas l'entrée sélectionnée dans la combobox), ce qui est le comportement le plus sûr et le plus prévisible. Supprimer l'entrée *sélectionnée* arbitrairement dans la liste ouvre une boîte de Pandore concernant les trous dans la numérotation et les dépendances parent-enfant.

------

**L'original est-il inclus dans l'Export ?**

EXPORT exporte toutes les mutations et retries de l'historique en fichiers .syx individuels. Mais l'utilisateur devrait probablement aussi pouvoir récupérer le patch original dans cet export — puisque COMPARE lui donne accès à ce patch et qu'il est potentiellement le meilleur point de référence. Une entrée `ORIG.syx` automatiquement incluse dans le dossier d'export me semble être une bonne pratique, sans aucun coût en termes de GUI.

------

**Auto-envoi lors de la navigation dans la combobox**

Tu précises que l'envoi SysEx se fait après MUTATE et RETRY. Mais qu'en est-il quand l'utilisateur sélectionne une entrée dans la combobox ? Si la sélection n'envoie pas automatiquement le patch au synthé, l'utilisateur doit faire une action supplémentaire pour l'auditionner, ce qui casse le workflow. Si elle l'envoie automatiquement, il faut protéger contre un scroll rapide dans la combobox qui spammerait le SysEx — une petite temporisation de 200-300ms après la dernière interaction suffit.

------

**COMPARE désactivé quand l'historique est vide**

Détail mineur mais important : si l'utilisateur vient juste de charger un patch et n'a encore rien muté, COMPARE n'a pas de sens (il n'y a rien à comparer puisque le patch courant *est* l'original). Le toggle devrait être désactivé dans cet état.

------

**Sur "DL" comme label**

Le label est court et pratique, mais "DL" est ambigu — Download dans les habitudes web/UI modernes. "DEL" sur trois lettres est plus universellement lisible dans un contexte audio et ne prend qu'un pixel de plus. C'est cosmétique mais ça peut éviter une confusion au premier coup d'œil.

------

En résumé, la question du comportement de MUTATE depuis une position non-terminale est la seule vraie lacune fonctionnelle restante. Tout le reste est de l'ordre du détail de polish. Le module est globalement très bien conçu pour les contraintes que tu as.

---

## Guillaume

Voici une mise à jour de mon mockup. J'ai renommé le bouton DL en DEL (avec un police plus petite pour que ça rentre), et c'est effectivement DELETE que DL signifiait dans mon esprit, et non pas DELETE LAST.

Et si je simplifiais mon système de nommage automatique des mutations et retries ? Chaque clic sur Mutate ou Retry pourrait alors générer un nouveau patch nommé simplement M1, M2, M3, etc. Après tout, un retry et aussi une mutation, c'est juste le point de départ en termes de valeurs de paramètres qui diffère. Il n'y aurait alors plus de notation du type Mi-Rj et donc plus de lien de parenté non plus entre un retry et la mutation qui lui a donné naissance. Je pourrais aussi limiter le nombre de mutations/retries à un nombre plus important, pourquoi pas 99 comme le nombre de patches par banque de ce synthé.

Avec cette nouvelle façon de voir les choses, le bouton DEL serait sans doute plus intuitif : il supprimerait tout simplement la mutation/retry actuellement sélectionnée dans la combobox, peut importe celle qui est sélectionnée. Cela ne me semble pas gênant si les numéros des mutations/retries restants ne sont plus consécutifs après des DEL. Au contraire, cela permet de se rappeler ce qui a été supprimé avec DEL. Et le bouton CLEAR purge intégralement l'historique, comme c'était prévu précédemment.

Tu as raison concernant la désactivation du toggle COMPARE lorsque l'historique est vide. Ce togglr sera d'ailleurs désactivé par défaut au chargement du plugin, mais aussi au chargement d'un nouveau patch, puisque l'historique sera purgé à chaque chargement de nouveau patch depuis la mémoire du synthé ou depuis un fichier .syx.

Tu as raison également de suggérer l'inclusion du patch original qui a donné naissance à toutes les mutations, lors de l'export de l'historique. Il pourra s'appeler Initial.syx, afin de se retrouver avant les patches mutés (M1.syx, M2.syx, etc.) dans l'ordre alphabétique (le I est avant le M).

Concernant la sélection dans la combobox, cela envoi instantanément le message SysEx de patch au synthé et il faut effectivement prévoir un délai entre chaque envoi. Je suis confronté à la même contrainte dans une autre combobox de mon module Computer Patches ou l'on peut naviguer dans les patches au format .syx depuis un dossier ouvert dans l'ordinateur.

---

## Claude IA

Le passage au modèle plat M1-M99 est une bonne décision globalement : ça simplifie le système de nommage, ça supprime la complexité de la hiérarchie dans la combobox, et ça offre plus de souplesse. Voici ce qui mérite encore réflexion.

------

**RETRY dans le modèle plat : la notion de "parent state" devient invisible mais reste indispensable**

C'est le point le plus subtil de cette simplification. Dans l'ancien modèle, le nom `M3-R2` disait implicitement *"pour générer un retry de cette entrée, on repart de l'état paramètres de M2"*. Dans le modèle plat, une entrée `M8` ne dit rien de son état de départ. Pourtant, RETRY a besoin de cette information pour fonctionner correctement : si M8 était un retry de M5 dans l'ancien sens, un RETRY sur M8 doit repartir de l'état de M4 — et non de l'état de M7.

Concrètement, chaque entrée de l'historique doit donc stocker en interne une référence à son *parent state* (les valeurs des paramètres au moment où la mutation a été lancée), en plus de ses propres valeurs. C'est une contrainte de data structure, pas de GUI. Mais ça soulève une question UX : quand l'utilisateur est sur M8 dans la combobox et presse RETRY, il ne sait plus nécessairement de quel état il va repartir. Dans l'ancien modèle, le nom lui donnait cette information.

Une solution légère sans coût en pixels : afficher dans ta barre de messages en bas un texte contextuel lors de la sélection d'une entrée, du type *"M8 — based on M4"*. L'information est là quand elle est utile, sans polluer la GUI.

------

**RETRY s'applique-t-il à l'entrée sélectionnée ou seulement à la dernière entrée ?**

Avec le modèle plat et un DEL qui opère sur n'importe quelle entrée, RETRY devrait logiquement aussi opérer sur l'entrée *sélectionnée* dans la combobox. Mais si tu es sur M5 alors que M6-M12 existent après, presser RETRY crée M13 depuis le parent state de M5 — et l'utilisateur ne le verra peut-être pas venir. Si au contraire RETRY n'opère que sur la dernière entrée de la liste (peu importe la sélection), le comportement est plus prévisible mais moins cohérent avec DEL. Il faut trancher.

------

**MUTATE depuis une position non-terminale : toujours non traité**

Je l'avais soulevé lors du dernier échange et tu ne l'as pas encore adressé. Avec le modèle plat et 99 slots, la question devient : si tu es sur M5 et que M6-M12 existent, presser MUTATE fait quoi ? Crée-t-il M13 depuis M5 (sans rien supprimer, mais en laissant une liste potentiellement incohérente en termes de workflow) ? Ou tronque-t-il l'historique à partir de M5 ? C'est la seule vraie lacune fonctionnelle qui reste ouverte.

------

**Tout le reste est en ordre**

`Initial.syx` avant `M1.syx` dans l'ordre alphabétique à l'export : c'est une belle attention. Le débounce sur la combobox tu l'as déjà résolu ailleurs dans ton plugin, donc tu as la pattern. Le comportement de COMPARE désactivé au chargement d'un nouveau patch est logique. Le modèle est solide.

---

À méditer...