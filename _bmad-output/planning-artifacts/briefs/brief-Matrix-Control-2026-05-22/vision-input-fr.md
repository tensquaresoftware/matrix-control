---
organization: Ten Square Software
project: Matrix-Control
title: Vision Input (French)
author: Guillaume DUPONT
language: fr
created: 2026-05-22
---

1. Vision

Mes utilisateurs disent : Wow ! Voilà un éditeur MIDI qui en a sous le capot et qui va véritablement donner une nouvelle vie à mon Matrix-1000 ! (ou mon Matrix-6/6R, censé avoir la même architecture et compatible en principe au niveau des messages SysEx). Moi qui pensait que le Matrix-1000 était une simple boite à presets avec ses 1000 sons, c'est en fait un vrai synthé programmable bourré de possibilités, et je redécouvre la bête grâce au Matrix-Control ! Non seulement le Matrix-Control fonctionne dans mon DAW habituel en tant que plugin, mais il peut aussi fonctionner en mode standalone et cerise sur le gâteau, je peux l'installer sous macOS, Windows et Linux ! Que demander de plus ?

Moi je dis : mes chers utilisateurs, je suis vraiment ravi de pouvoir vous apporter ce plugin dont je suis très fier, qui non seulement va vous permettre de programmer votre Matrix-1000 comme jamais vous ne pensiez pouvoir le faire (accès ultra rapide et direct à toutes les fonctions, via une UI claire et bien pensée), mais vous allez pouvoir allez encore plus loin avec votre synthé grâce à des fonctionnalités bonus que j'ai mis du cœur à intégrer : clipboard intelligent, initialisation rapide des modules et presets, affichage graphique interactif des enveloppes et du Track Generator, sauvegarde des patches utilisateur sur le disque de votre ordinateur pour un nombre quasi illimité de patches et un rangement ergonomique de vos collections de patches, et cerise sur le gâteau : le Patch Mutator pour générer des patches au hasard ou muter un patch existant ! Avec mon Matrix-Control, vous allez redécouvrir votre synthé et vous allez certainement le remettre au cœur de vos productions musicales ! Si vous ne possédez pas encore de Matrix-1000/6/6R, il y a fort à parier que vous allez chercher à en acquérir un ! 😁

2. Pourquoi BMad maintenant ?

Ma passion pour la musique et l'audionumérique remonte à mon adolescence. Bien que je sois un simple développeur amateur, j’ai acquis quelques années d’expérience en tant que développeur hard/soft dans les années 2000 (domaine médical et industriel, applications Windows et systèmes embarqués avec cartes contrôleurs). Lorsque j'ai commencé à me pencher sur mon projet Matrix-Control, j’ai immédiatement perçu la complexité de ce type de projet et la difficulté de le mener à bien, malgré ma rigueur et ma méthodologie, héritées de ma formation scientifique, de mon état d’esprit général et de mon amour du travail bien fait.

J’ai lancé le projet Matrix-Control à l’été 2025 suite à un arrêt de travail pour mon opération du canal carpien. J'avais du temps de libre devant moi et je recevais à l'époque diverses demandes de mes utilisateurs de mon device Max for Live "Matrix-1000 Editor". Après avoir découvert le potentiel de l’intelligence artificielle (ChatGPT) pour le codage en juillet 2025, je me suis au départ que je pourrais sans doute déboguer plus facilement mon Matrix-1000 Editor, plutôt que de tout reprendre à la main.  Cependant, j’ai rapidement réalisé que mon code JS de l'époque était en fait assez bancal et surtout très peu documenté, et que j'allais m'embourber dans une galère monstre pour faire refonctionner mon device sous le nouveau Max 9, visiblement assez différent de Max 8 sous lequel je l'avais conçu...

J'en ai assez rapidement conclu qu’il serait finalement préférable de créer un véritable plugin VST/AU compatible avec tous les DAW du marché, plutôt que de vouloir m'acharner avec ce device M4L qui battait de l'aile et qui resterait réservé aux utilisateurs d'Ableton Live.  Puisque je serais amené à consacrer un temps et des efforts non négligeables pour le débogage de cet éditeur, sans véritable garantie de succès en raison de la mauvaise qualité de ma documentation et de la structure initiale du projet, j’ai pensé qu’il valait mieux investir ce temps dans la création de quelque chose de propre.  

Néanmoins, j’étais conscient dès le départ de mes lacunes en C++ et en génie logiciel. Je n'avais plus programmé depuis 20 ans, même si je m'étais remis un peu à jour pour le plaisir entre temps, via une formation en ligne assez pointue sur udemy.com. Ma maîtrise de la gestion de projet logiciel et de la documentation est loin des standards profesionnels. J'ai découvert le format markdown il y a quelques mois à peine, et je découvre seulement toutes les facettes du métier de devs avec les personas BMad, c'est incroyablement complexe !. Lors de mes premiers essais de vibe-coding avec ChatGPT, j’ai brièvement cru que l’assistance de l’IA pourrait m’aider à surmonter ces difficultés...

Puis j'ai découvert Visual Studio Code et ensuite Cursor que j'adore et que j'ai adopté. Je dois admettre que lorsque j’ai commencé à coder avec l’aide de Cursor et mon petit abonnement à 20 $/mois, j’ai été stupéfait par le potentiel de l’IA dans ce type de projet. Comme je l'ai dit, j’ai commencé mon projet avec ChatGPT, en utilisant de simples discussions dans le chat et en copiant-collant de nombreux extraits de code qu’il a générés. Au début, c’était un vrai casse-tête de tester tous ces extraits. Cependant, prendre en main un vrai IDE tel que Cursor, orienté Agentic, a été un véritable tournant.

Après quelques mois de développement, même si j’ai fait des progrès significatifs avec cette approche, j’ai commencé à ressentir les limites du vibecoding. Cela est devenu particulièrement évident lorsque j’ai rencontré des difficultés avec mon interface utilisateur et que j’ai voulu mettre en place une fonctionnalité de mise à l’échelle (User UI Scaling). J’ai été assez déçu par les résultats obtenus avec les flous et les zones de clipping à l’écran, et les problèmes de performance de mon plugin. De plus, la masse de code a augmenté de façon exponentielle, tout comme la documentation et les nombreux fichiers Markdown générés par l’IA. Je me suis retrouvé submergé par cette quantité de données et j’ai eu beaucoup de mal à progresser. Le projet est devenu une source de stress et de frustration, voyant que je n'avais plus à la bonne vitesse.

C’est alors que j’ai découvert la méthode BMad, un peu par hasard, grâce à une vidéo sur YouTube. J’ai failli la tester immédiatement avec le projet Matrix-Control, mais j’ai eu la présence d’esprit de me dire qu’il valait mieux la tester d’abord sur un petit projet simple. Cela me permettrait de voir si elle répondait vraiment à mes attentes et si elle pouvait réellement produire le résultat que j’espérais dès le départ : un plugin impeccable (Core/UI/UX) avec un code impeccable (bug free, standards recommandés par l'Oncle Bob), un repos impeccable et une documentation professionnelle qui pourrait servir d’exemple à des étudiants en informatique.

J’ai donc brièvement mis de côté le projet Matrix-Control pour me lancer dans le projet Sparkay, que j’ai terminé en seulement trois semaines.  Si l’on cumule les heures passées, on est plutôt à deux semaines. La méthode BMad s’est avérée incroyablement puissante et productive pour ce projet, surtout que je me suis délibérément aventuré dans un domaine inconnu : le développement d’applications mobiles. Sparkay est une application pour iPhone et Android, et je suis stupéfait de la rapidité et de l’efficacité avec lesquelles j’ai pu progresser grâce à la méthode. 

Au passage, j’ai donc réalisé deux objectifs en un. Le projet que j’ai développé avec Sparkay est en réalité l’aboutissement d’une idée qui me trottait dans la tête depuis 20 ans.  À cette époque, j’ai commencé à avoir du mal à composer de la musique, paralysé par le syndrome de la page blanche. Malgré cela, j’ai continué à écouter beaucoup de musique, que ce soit en voiture, dans le bus ou en balade avec mon iPhone et mes écouteurs. J’ai toujours voulu pouvoir noter les idées musicales qui me venaient à l’esprit en écoutant ces morceaux, afin de m’en inspirer plus tard dans mon studio. Aujourd’hui, j’ai enfin créé Sparkay. C’est une double victoire : mon projet est devenu réalité et je peux l’utiliser comme je l’avais imaginé il y a 20 ans. De plus, cela m’a permis de tester la méthode BMad avec succès, et je compte maintenant propulser mon projet Matrix-Control avec BMad et Cursor. Je suis ravi de relancer ce projet avec ton aide, en intégrant BMad comme garde-fou et en étant totalement confiant quant au double succès du Matrix-Control : code au top, repo & docs au top également.

Concernant ta question sur le vibecoding, cela m’a effectivement pris un certain temps. J’ai dû jongler mentalement avec cette masse de code en constante expansion, tout en essayant de faire le lien avec les documents que j’avais initialement envisagés d’écrire en anglais.  Rapidement, j’ai décidé de les traduire en français, car la quantité d’informations à gérer était trop importante pour un seul cerveau. De plus, j’ai un travail salarié à temps plein qui m’occupe quatre jours par semaine et me demande beaucoup d’énergie.

Par conséquent, j’ai travaillé sur le projet quelques heures le soir et un peu le week-end. Cela a naturellement engendré du stress, surtout lorsque je n’avançais pas, et j’ai constaté une baisse progressive de la qualité du code en raison de sa complexité.  J’ai progressivement dévié de mon objectif initial, qui était de produire un code impeccable, digne de l’oncle Bob, ainsi qu’une documentation irréprochable.  Aujourd’hui, la documentation est probablement assez bancale et nécessitera une révision complète, du cahier des charges au reste.

3. Utilisateur #1

Il aurait une préférence pour les synthés hardware, possèderait un Oberheim Matrix-1000 qu'il utilisait jusqu'ici comme une simple boite à sons (ce qu'il n'est pas) et dont il connaît le potentiel de synthèse incroyable, mais surtout (et malheureusement) l'impossibilité de le programmer en façade car il faut soit un Matrix-6 ou Matrix-6R (ses grand frères) pour le faire en MIDI, soit un éditeur MIDI hard ou soft. Cet utilisateur aura sans doute testé les divers éditeur MIDI hard ou soft conçus pour ce synthétiseur légendaire, mais aucun ne lui aura sans doute donné pleinement satisfaction ni l'effet "Wow !".

Cet utilisateur aurait au minimum besoin de pouvoir utiliser le Matrix-Control comme une télécommande dédiée et s'amuser rapidement et sans frictions avec les paramètres de son Matrix-1000 (il y en a beaucoup), afin de modeler la matière sonore et trouver de nouveaux sons ou nouvelles idées pour ses productions musicales. Mais ça c'est vraiment la base...

4. Périmètre v1

Le minimum vital pour mon Matrix-Control que je souhaite intégrer en v1 est plus ambitieux. Mon plugin sera disponible sous la forme d'un plugin VST/AU compatible macOS, Windows et Linux, mais aussi sous la forme d'une application standalone. Lorsque je parle de mon "plugin", je parle aussi implicitement de l'app standalone qui propose les mêmes fonctionnalités.

Le Matrix-Control sera certes en mesure de piloter le Matrix-1000 (compatiblité Matrix 6/6R à entrevoir sans doute en v2, avec notamment l'ajout du mode SPLIT) en se comportant comme cette télécommande logicielle décrite plus haut, et en couvrant donc intégralement les modes PATCH (= sections PATCH EDIT et MATRIX MODULATION) et MASTER (= section MASTER EDIT) du synthé.

Mon plugin intègrera également de nombreux bonus que j'ai imaginés à l'époque de mon "Matrix-1000 Editor" et que j'ai complété récemment lors de l'étude du Matrix-Control. Ces bonus augmentent considérablement le potentiel du synthé ou plutôt les capacités à le programmer plus vite, avec beaucoup moins de friction qu'en utilisant par exemple un Matrix-6/6R (car ça reste une belle galère apparemment !).

L'utilisateur doit pouvoir :

- Charger à distance un preset depuis une mémoire du synthé (sélection d'un numéro de banque et d'un numéro de patch depuis l'UI du plugin, parmi les 1000 patches disponibles)
- Visualiser dans l'UI l'état de tous les paramètres du patch chargé dans le plugin (Sliders, ComboBoxes, etc.)
- Visualiser dans l'UI le nom du patch chargé dans le plugin (il est encodé sur 8 caractères dans le message SysEx), grâce à un afficheur dédié PATCH NAME dans l'UI. Il faut noter que cet afficheur de nom de patch est propre à mon Matrix-Control, il n'existe pas en tant que tel dans le Matrix-1000
- Manipuler en temps réel et à distance tous les paramètres des modes PATCH (sections PATCH EDIT et MATRIX MODULATION) et MASTER du synthé (mode SPLIT propre au Matrix-6/6R, à intégrer probablement en v2)
- Modifier le nom du patch courant via un double-clic sur l'afficheur PATCH NAME (ou peut-être via un bouton EDIT, à discuter...), lorsque la banque courante est de type RAM/User (banque 0 ou 1), de façon à pouvoir modifier les données du patch courant qui codent ce nom de patch

I. Mode PATCH

1. Section PATCH EDIT = 10 modules de synthèse :

- DCO 1 (10 paramètres)
- DCO 2 (10 paramètres, quasi identique à DCO 1)
- VCF/VCA (10 paramètres)
- FM/TRACK (9 paramètres)
- RAMP/PORTAMENTO (9 paramètres)
- ENV 1 (10 paramètres)
- ENV 2 (10 paramètres, identique à ENV 1)
- ENV 3 (10 paramètres, identique à ENV 1 et ENV 2)
- LFO 1 (9 paramètres)
- LFO 2 (9 paramètres, quasi identique à LFO 1)

Ces 10 modules de synthèse seront complétés par 3 afficheurs interactifs pour le dessin des enveloppes ENV 1/2/3, un afficheur interactif pour le dessin de la fonction de transfert du TRACK GENERATOR et l'afficheur PATCH NAME avec son en-tête "PATCH NAME" permettant d'afficher le nom du patch courant (si j'opte pour un bouton EDIT, il serait placé à droite dans cet en-tête).

2. Section MATRIX MODULATION = 10 bus de modulation

- Paramétrer les 10 bus de modulation (0 à 9) de la matrice de modulation, avec pour chacun des bus un trio de paramètres Source / Amount / Destination

Note : Toutes les commandes (Sliders ou ComboBoxes) qui équipent ces modules de synthèse et ces bus de modulation sont directement associées à des paramètres du synthé (ex : Slider FREQUENCY du module VCF/VCA, ComboBox DESTINATION du bus de modulation n° 7, etc.) Il s'agit de "Synth Parameter Widgets", c'est ainsi que je les qualifient dans le code C++ actuel

3. Section PATCH MANAGER

Cette section n'existe pas en tant que tel dans le synthé, c'est un regroupement de nouvelles fonctionnalités que j'ai imaginées pour mon plugin Matrix-Control, avec ce nom de section que j'ai inventé.

Cette nouvelle section complète le mode PATCH du synthé, elle comporte les modules suivants : BANK UTILITY, INTERNAL PATCHES, COMPUTER PATCHES et PATCH MUTATOR. Les commandes (Boutons, NumBoxes, Sliders, ComboBoxes, etc.) qui constituent ce PATCH MANAGER ne sont pas directement associés à des paramètres du synthé et sont donc des widgets autonomes. Il s'agit de "Standalone Widgets" : c'est ainsi que je les qualifient dans le code C++ actuel.

3.1. Module BANK UTILITY : ce module permet à l'utilisateur de :

- Naviguer de manière directe dans les 10 banques de patches du synthé, via un panneau BANK SELECTOR équipé de boutons 0 à 9 (on retrouve ces boutons sur la façade du Matrix-1000)
- Verrouiller à distance la banque courante via un bouton BANK LOCK (on retrouve ce bouton sur la façade du Matrix-1000 ; dans le code C++ actuel j'avais plutôt opté pour un bouton UNLOCK, j'aimerais changer cela pour me rapprocher un peu plus de l'ergonomie initiale du Matrix-1000)

3.2. Module INTERNAL PATCHES : ce module permet à l'utilisateur de :

- Naviguer de manière cyclique dans les 100 patches (numérotés 0 à 99) de la banque courante lorsqu'elle est verrouillée, via des boutons [<] (= "Previous Patch") et [>] (= "Next Patch"). Une numbox "Bank Num" affiche (en lecture seule) le numéro de banque courante, avec présence d'un point en bas à droite du numéro lorsque la banque est verrouillée (comme sur l'afficheur du Matrix-1000). Une numbox "Patch Num" affiche le numéro de patch courant et permet aussi de l'éditer via un double clic, puis en renseignant directement sa valeur (avec vérification des valeurs autorisées)
- Naviguer de manière cyclique dans les 1000 patches du synthé (banque 0 / patches 0 à 99 puis banque 1 / patches 0 à 99, etc.) si aucune banque n'est verrouillée, au moyen des mêmes boutons et numboxes que lorsque la banque courante est verrouillée (pas de présence de point dans la numbox Bank Num dans ce cas)
- Initialiser le patch courant via un bouton INIT, sur la base d'un fichier de patch nommé INIT.syx s'il est présent dans un dossier utilisateur configurable dans les Settings du plugin, sinon sur la base de valeurs hardcodées dans le plugin (regroupées dans un fichier .h centralisé)
- Copier le patch courant dans le clipboard du plugin, via un bouton COPY
- Coller le patch présent dans le clipboard du plugin vers le patch courant, via un bouton PASTE disponible lorsque la banque courante est de type RAM/User (banque 0 ou 1), afin de pouvoir l'enregistrer ensuite dans une mémoire du synthé
- Enregistrer le patch courant dans une mémoire du synthé via un bouton STORE, disponible lorsque la banque courante est de type RAM/User (banque 0 ou 1)

3.3. Module COMPUTER PATCHES : ce module permet à l'utilisateur de :

- Ouvrir via un bouton OPEN un dossier de l'ordinateur contenant des patches sauvés sous forme de fichiers .syx. Ces fichiers .syx doivent être directement compatibles avec le synthé, ils seront donc également transférables en l'état au Matrix-1000 via une application telle que SysEx Librarian pour macOS
- Naviguer de manière cyclique dans les fichiers de patches (valides) ouverts depuis ce dossier, via des boutons [<] (= "Previous Patch File") et [>] (= "Next Patch File")
- Naviguer de manière directe dans les fichiers de patches (valides) ouverts depuis ce dossier, via une ComboBox listant les noms de ces fichiers valides trouvés dans le dossier
- Afficher dans l'afficheur PATCH NAME de l'UI le nom du dernier fichier de patch chargé. Dans le code JS de mon Matrix-1000 Editor, je me basais uniquement sur le nom de fichier .syx. Dans mon Matrix-Control, je souhaite me rapprocher un peu plus de l'architecture du Matrix-1000 et exploiter pleinement le nom de patch qui est stocké au sein même des données SysEx. Celcui-ci est codé sur 8 caractères dans les données du patch, et il peut éventuellement différer du nom du fichier .syx, celui-ci ayant pu être renommé entre temps par l'utilisateur en dehors du plugin. en chargeant un tel fichier, le plugin proposera à l'utilisateur soit de renommer le fichier .syx sur la base du nom stocké dans les données de patch, soit de renommer le patch (et donc modifier ses données internes) sur la base du nom du fichier .syx, à condition qu'il respecte les standards imposés par le Matrix-1000 (8 caractères ASCII, certains caractères sont interdits)
- Persister le chemin du dernier dossier ouvert, afin de le réexploiter sans friction lors de la prochaine session avec le plugin ou l'app standalone
- Sauver via un bouton "SAVE AS..." le patch en cours d'édition vers un fichier .syx dans un dossier de l'ordinateur. Une boite de dialogue permet à l'utilisateur d'indiquer le dossier de son choix ainsi qu'un nom de fichier pour son patch. Le nom du fichier sera contraint par certaines règles (taille du nom, caractères interdits, etc.) permettant ensuite de stocker correctement ce nom dans les données du patch et de l'afficher dans le champ PATCH NAME du plugin
- Sauver le patch en cours d'édition via un bouton SAVE, sans avoir à repréciser le nom de fichier si celui-ci a déjà été renseigné lors d'une première sauvegarde via le bouton "SAVE AS..."

3.4. Module PATCH MUTATOR : ce module permet à l'utilisateur de :

- Muter aléatoirement un patch, à la manière du "Sound Mutation" du plugin Absynth 5 de Native Instruments, en indiquant via des Toggles quels modules de synthèse du Matrix-1000 doivent être affectés et en paramétrant un taux de mutation (Amount en %) ainsi qu'un taux de dérive aléatoire (Random en %)
- Un bouton MUTATE lance la mutation du patch courant, selon les réglages établis dans le module PATCH MUTATOR. Un deuxième clic sur le bouton MUTATE mute à nouveau le patch, depuis sa précédente mutation, cumulant ainsi l'effet du PATCH MUTATOR sur ce patch
- Un bouton RETRY complète le bouton MUTATE. Il est initialement inactif et devient cliquable après une première mutation via MUTATE. Un clic sur RETRY lance une nouvelle mutation du patch mais depuis son état précédent, sans effet cumulatif du PATCH MUTATOR. Ceci permet donc de tester diverses mutations depuis la même base de patch, pour essayer des variantes de mutation (surtout en modifiant le Slider Random entre deux essais) mais sans accumuler les effets de ces mutations successives, comme le ferait le bouton MUTATE
- Les diverses mutations des patches (MUTATE ou RETRY) apparaissent progressivement en liste dans une ComboBox HISTORY, celle-ci affiche EMPTY lorsqu'elle est vide
- Un maximum de 100 mutations est autorisé par le plugin, elles sont numérotées 00 à 99
- Les noms des mutations de patch sont "M00" à "M99" dans la ComboBox HISTORY ainsi que dans le champ PATCH NAME
- Un bouton COMPARE permet à tout moment de comparer le patch chargé initialement dans le plugin (depuis le synthé ou un fichier .syx) et le patch dernièrement muté, ou issu de la ComboBox HISTORY
- Un bouton "–" (signifiant "Delete", je manque de place dans l'UI...) permet de supprimer la mutation couramment sélectionnée dans la ComboBox HISTORY, le patch initialement chargé est rechargé à la place
- Un bouton CLEAR permet de purger intégralement la ComboBox HISTORY, celle-ci affiche ensuite à nouveau EMPTY
- Un bouton EXPORT permet d'exporter toutes les patches mutés listés dans la ComboBox HISTORY, en générant un fichier .syx pour chacun d'entre eux. Une boite de dialogue permet à l'utilisateur d'indiquer le dossier de son choix pour l'export ainsi qu'un nom de base de fichier, qui sera automatiquement complété par le suffixe "Mxx" (les contraintes sur ce nom de base seront à définir, compte tenu des contraintes sur les noms de patches imposées par le Matrix-1000, je ne suis pas encore bien fixé sur ce point...)

Note : Je ne suis pas encore totalement sûr du comportement final de ce module PATCH MUTATOR, ni de la pertinence ou complétude de ce que j'ai imaginé ici. Une session de brainstorming avait été lancée avec l'IA à ce sujet il y a quelques temps. Il faudra sans doute y revenir ensemble avant d'implémenter toutes les fonctionnalités de ce module un peu particulier (voir mon fichier /Volumes/Guillaume/Dev/SDKs/JUCE/Projects/Matrix-Control/Documentation/Development/GUI/Patch-Mutator-Brainstorming.md)

II. Mode MASTER

Section MASTER EDIT = 3 modules de paramétrage global du synthé

- MIDI (8 paramètres)
- VIBRATO (7 paramètres)
- MISC (signifie "Miscellaneous" ; 7 paramètres)

Note : Toutes les commandes (Sliders ou ComboBoxes) qui équipent ces modules de paramétrage global sont directement associées à des paramètres du synthé (ex : Slider SPEED du module VIBRATOR, ComboBox UNISON du module MISC, etc. Il s'agit donc là aussi de "Synth Parater Widgets", tel qu'indiqué dans le code C++ actuel)

III. BONUS

La v1 de mon Matrix-Control comportera également quelques fonctionnalités bonus qui constituent une véritable valeur ajoutée pour les utilisateurs du Matrix-1000 :

- Boutons "I" (INIT) permettant d'initialiser rapidement certains éléments clés du synthé. Ces boutons sont présents dans les en-têtes des 10 modules de synthèse de PATCH EDIT (VCO 1, VCO 2, etc.), dans l'en-tête de la section MATRIX MODULATION (initialisation complète des 10 bus de modulation, en un clic), dans chacun des 10 bus de modulation (initialisation d'un bus en particulier) et dans les en-têtes des 3 modules de MASTER EDIT (MIDI, VIBRATO et MISC). Les valeurs utilisées pour la réinitialisation des paramètres du mode PATCH sont celle du fichier INIT.syx s'il est présent dans le dossier utilisateur paramétré dans les Settings du plugin, sinon celles hardocodées dans le plugin. Les valeurs utilisées pour la réinitialisation des paramètres du mode MASTER sont hardcodées dans le plugin (à voir plus tard si on prévoit aussi un fichier INIT.syx pour cela, ça pourrait être intéressant...)
- Boutons "C" (COPY) et "P" (PASTE) permettant respectivement de copier les valeurs des paramètres d'un module spécifique (ex : ENV 1) pour les coller ensuite vers un module du même type (ex : ENV 2 ou ENV 3). Le clipboard du Matrix-Control est donc "Type Aware", les boutons "C" sont toujours actifs, les boutons "P" sont initialement inactifs et deviennent actifs uniquement dans les modules compatibles avec le contenu du clipboard. Les boutons C & P sont disponibles dans les en-têtes des modules DCO 1, DCO 2, ENV 1, ENV 2, ENV 3, LFO 1 et LFO 2 de PATCH EDIT. Bien que les modules DCO 1 et DCO 2 ne soient pas strictement identiques dans leurs listes de paramètres, ils sont compatibles entre eux via les boutons C et P, et le collage ignore alors les paramètres qui diffèrent. Idem concernant les modules LFO 1 et LFO 2 qui diffèrent un peu. En revanche, on peut noter que les modules ENV 1, ENV 2 et ENV 3 sont strictement identiques dans leurs listes de paramètres

Voici quelques compléments d'infos pour en terminer sur le périmètre de la v1 du Matrix-Control :

Interface Utilisateur : elle est divisée en 3 parties horizontales :

- Une bande en haut de la fenêtre (voir "HeaderPanel" dans le code actuel), comportant des éléments tels que le logo "MATRIX-CONTROL" du plugin (à l'extrêmité gauche de la zone), des ComboBoxes "MIDI INPUT" et "MIDI OUTPUT" permettant de sélectionner les ports MIDI d'entrée et sortie pour la communication bidirectionnelles avec le Matrix-1000, des témoin lumineux indiquant l'activité MIDI en entrée et en sortie. Un clic sur le logo MATRIX-CONTROL donne accès au réglage du "UI Scaling" (facteurs de mise à l'échelle retenus : 50%, 75%, 100%=défaut, 125%, 150%, 175% et 200%) et au thème du plugin BLACK (= défaut) ou CREAM. En plus de toutes ces options, peut-être aurons-nous besoin d'ajouter un bouton SETTINGS pour d'autres réglages plus avancés du plugin
- Une zone centrale occupant la majeure partie de la fenêtre (voir "BodyPanel" dans le code actuel), affichant les sections PATCH EDIT, MATRIX MODULATION, PATCH MANAGER et MASTER EDIT. Ma UI est maintenant bien au point dans ma tête et je l'ai simulée au pixel près avec Figma, je te fournirai une capture d'écran en guise de base de travail et de référence
- Une bande en bas de la fenêtre (voir "FooterPanel" dans le code actuel), permettant au plugin d'afficher des messages (infos, avertissements, erreurs, etc.) à l'attention de l'utilisateur. Par exemple, lors de l'ouverture d'un dossier contenant des fichiers de patch au format .syx, une vérification de ces fichiers est lancée par le plugin qui peut ensuite afficher dans cette zone combien de fichers .syx ont été trouvés, combien sont valides, etc.

Fonctionnalités que j'aimerais également implémenter dans la v1 du Matrix-Control :

- Automatisation de tous les paramètres des mode PATCH et MASTER dans le DAW, avec cependant une limitation à prévoir au niveau du nombre de messages SysEx transmis au synthétiseur. En effet, le Matrix-1000 possède un buffer MIDI d'entrée de taille modeste et il a tendance à planter lorsqu'il reçoit trop de messages MIDI/SysEx rapprochés dans le temps
- Détection automatique du statut de connexion MIDI (entrée et sortie) du Matrix-1000, avec affichage d'un message d'erreur dans le FooterPanel si une connexion semble défaillante en entrée ou sortie (exemple : détection d'un câble MIDI défectueux, ou d'un plantage du synthé qui ne répond plus, etc.)
- Interrogation à distance du numéro de version du firmware du Matrix-1000 et affichage dans la partie droite du FooterPanel
- ComboBox "Audio From" à placer dans le HeaderPanel, permettant d'indiquer à quelle entrée physique de l'interface audio est connectée la sortie audio du Matrix-1000. Cela éviterait d'avoir à créer deux pistes dans le DAW : une piste MIDI pour instancier le plugin Matrix-Control et assurer la communication MIDI en entrée/sortie avec le Matrix-1000, et une piste audio pour récupérer le flux audio produit par le synthétiseur. C'est de cette façon que je recommandais à mes utilisateurs de procéder avec mon Matrix-1000 Editor dans Ableton Live (mon DAW fétiche). Dans Ableton Live, une alternative est d'utiliser un device "External Instrument" pour n'utiliser au final qu'une seule piste (MIDI en l'occurence), mais je ne sais pas si ce genre d'outil existe dans les DAW concurrents. La façon dont mon plugin gèrera le MIDI et éventuellement l'audio est un sujet qui me préoccupe et qui n'est pas encore totalement tranché. En effet, prévoir une piste MIDI dédiée pour instancier le plugin et gérer la communication bidirectionnelle avec le Matrix-1000 présente l'avantage de pouvoir être configurée en mode "Monitor = In" dans Ableton Live, permettant ainsi à l'utilisateur de ne plus avoir à se soucier de l'état d'armement de cette piste MIDI pour autoriser ou non la réception des messages SysEx depuis le synthétiseur. Avec ce mode de fonctionnement, l'éditeur MIDI est donc toujours opérationnel, quoi que l'on fasse dans les autres pistes du DAW. Sans cette option "Monitor = In", s'il faut jongler avec le bouton d'armement de la piste MIDI pour autoriser la réception des messages SysEx envoyés par le Matrix-1000, cela peut vite devenir une galère car l'armement d'une autre piste dans le DAW a tendance à désarmer la piste MIDI actuellement armée (comportement exclusif, qui peut certes être désactivé dans les réglages de Live). Je ne sais pas comment ces aspects sont gérés dans les DAW concurrents, et il me faut trouver un mode opératoire qui fonctionne partout, ainsi qu'avec mon app en mode standalone ! Si mon plugin est amené à nécessiter une seule piste au lieu de deux dans le DAW, cela permettrait à l'utilisateur de gérer ses séquences de notes MIDI ainsi que ses courbes d'automation (envoyées au Matrix-1000 sous forme de messages SysEx) dans le même clip MIDI, ce qui s'avère très pratique pour synchroniser plus facilement par exemple une phrase de notes MIDI montantes avec une courbe d'automation montante sur le filtre du synthé
- Bouton "Help" ou "Read the manual..." ouvrant une documentation utilisateur que je rédigerai avec ton aide

[Fin du fichier Vision.md]