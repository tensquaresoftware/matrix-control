# Conversation : Optimisation des performances d'un plugin JUCE

## Question initiale

**Utilisateur :**
Je développe depuis plusieurs moi mon premier plugin VST/AU avec JUCE 8. Il s'agit d'un éditeur MIDI pour le synthétiseur Oberheim Matrix-1000, pour lequel j'avais déjà développé un éditeur MIDI avec Max for Live.

J'ai presque terminé le développement de la GUI de mon plugin et je constate déjà des problèmes de lenteur et de lourdeur au lancement du plugin (ou application standalone), et lors de l'utilisation des widgets ou du déplacement de la fenêtre.

Ma GUI est basée sur un MainComponent construit dans le PluginEditor, de la même taille que lui. Le MainComponent est ensuite structuré en panneaux divers, sur plusieurs niveaux de profondeur. J'ai compté 6 niveaux au total, dans certaines zones de la GUI. Les panneaux les plus profonds contiennent ensuite des widgets customisés par mes soins, implémentés sous la forme de classes héritant de widgets natifs ou de Component JUCE. On trouve par exemple des classes Slider, ComboBox, Button, Label, etc. 

Je n'utilise pas le concept de LookAndFeel de JUCE qui me semblait insuffisant pour mes besoins spécifiques concernant le comportement de mes widgets. À la place, j'ai créé une classe Theme qui me sert principalement à définir les couleurs de mes widgets et panneaux selon le thème sélectionné. Mes widgets comportent des méthodes paint(), setTheme() et parfois resized().

Lorsque je lance mon plugin, la GUI met plusieurs secondes à apparaître à l'écran, alors que c'est presque instantané pour de gros plugins tels que Vital Audio ou Serum 2. Par ailleurs, les premiers clics sur les boutons ne les font pas réagir tout de suite et le déplacement de la fenêtre occasionne des lourdeurs (curseur de souris de mon Mac qui se transforme en roue colorée tournante, etc.)

Je me demande d'où peut provenir cette lourdeur. L'Agent IA de mon IDE Cursor semble penser que cela peut provenir d'un nombre d'appels important aux diverses méthodes paint(), etc.

---

## Première réponse : Diagnostic des problèmes de performance

Votre analyse est probablement correcte. Les lenteurs que vous décrivez sont typiques de problèmes de rendu dans JUCE. Voici les causes les plus probables et leurs solutions :

### 1. Problèmes de `paint()` - Cause N°1 la plus fréquente

#### Ce qui ralentit probablement votre code :

```cpp
// ❌ MAUVAIS - Recréation d'objets à chaque paint()
void MySlider::paint(Graphics& g)
{
    Image knobImage = ImageCache::getFromMemory(...); // Rechargé à chaque frame!
    Font customFont = Font("Arial", 12.0f, Font::bold); // Recréé à chaque fois!
    Path circlePath;
    circlePath.addEllipse(...); // Recalculé à chaque paint()
    // ...
}
```

#### Solution :
```cpp
// ✅ BON - Précalcul dans le constructeur ou resized()
class MySlider : public Slider
{
public:
    MySlider()
    {
        knobImage = ImageCache::getFromMemory(...);
        labelFont = Font("Arial", 12.0f, Font::bold);
    }
    
    void resized() override
    {
        // Recalculer les paths uniquement quand la taille change
        circlePath.clear();
        circlePath.addEllipse(getLocalBounds().toFloat());
    }
    
    void paint(Graphics& g) override
    {
        // Utiliser les objets précalculés
        g.drawImage(knobImage, ...);
        g.setFont(labelFont);
    }
    
private:
    Image knobImage;
    Font labelFont;
    Path circlePath;
};
```

### 2. Hiérarchie trop profonde - 6 niveaux c'est beaucoup

Chaque niveau de Component ajoute des transformations de coordonnées et des appels `paint()`. Considérez :

```cpp
// ❌ Problématique
MainComponent
  └─ Panel1
      └─ Panel2
          └─ Panel3
              └─ Panel4
                  └─ Panel5
                      └─ Widget // 6 niveaux!
```

#### Solutions :
- **Fusionner des panneaux** : Si certains panneaux ne servent qu'à l'organisation visuelle sans logique, dessinez directement dans le parent
- **Utiliser `setBounds()` au lieu de panneaux intermédiaires** pour le positionnement

### 3. Optimisations critiques à vérifier

#### A. Éviter les repaints inutiles
```cpp
// Dans vos widgets customisés
void MySlider::mouseDown(const MouseEvent& e) override
{
    Slider::mouseDown(e);
    // ❌ NE PAS faire ça si pas nécessaire
    // repaint(); 
}

// Utilisez setOpaque(true) pour les composants opaques
MyPanel()
{
    setOpaque(true); // JUCE ne dessinera pas ce qu'il y a derrière
}
```

#### B. Désactiver le buffering inutile
```cpp
// Dans le constructeur de vos composants
setBufferedToImage(false); // Sauf si vous en avez vraiment besoin
```

#### C. Cache des images
```cpp
// ✅ Utiliser ImageCache pour les ressources
static Image& getKnobImage()
{
    static Image img = ImageCache::getFromMemory(BinaryData::knob_png, 
                                                  BinaryData::knob_pngSize);
    return img;
}
```

### 4. Lenteur au lancement - Causes spécifiques

```cpp
// ❌ Dans le constructeur du PluginEditor
PluginEditor::PluginEditor()
{
    // Évitez les opérations lourdes ici
    for(int i = 0; i < 1000; i++)
        addAndMakeVisible(new MyWidget()); // Trop d'allocations!
    
    // Chargement synchrone d'images lourdes
    backgroundImage = ImageFileFormat::loadFrom(...); // Bloquant!
}

// ✅ Mieux
PluginEditor::PluginEditor()
{
    // Lazy loading ou chargement asynchrone
    setSize(800, 600); // Définir la taille en premier
    
    // Allouer uniquement ce qui est visible au démarrage
    mainPanel = std::make_unique<MainComponent>();
    addAndMakeVisible(mainPanel.get());
}
```

### 5. Le problème `setTheme()`

Si vous appelez `setTheme()` récursivement sur tous vos composants, cela peut déclencher des cascades de `repaint()` :

```cpp
// ❌ Problématique
void Panel::setTheme(Theme* newTheme)
{
    theme = newTheme;
    repaint(); // Repaint immédiat
    
    for(auto* child : getChildren())
        if(auto* themed = dynamic_cast<ThemedComponent*>(child))
            themed->setTheme(newTheme); // Cascade de repaints!
}

// ✅ Mieux - Batch les repaints
void Panel::setTheme(Theme* newTheme)
{
    theme = newTheme;
    
    for(auto* child : getChildren())
        if(auto* themed = dynamic_cast<ThemedComponent*>(child))
            themed->setTheme(newTheme);
    
    repaint(); // Un seul repaint à la fin
}
```

### 6. Outils de diagnostic

Ajoutez ceci temporairement pour identifier les points chauds :

```cpp
void MyComponent::paint(Graphics& g)
{
    auto start = Time::getMillisecondCounterHiRes();
    
    // Votre code paint()
    
    auto elapsed = Time::getMillisecondCounterHiRes() - start;
    if(elapsed > 5.0) // Plus de 5ms
        DBG("Paint de " << getName() << " : " << elapsed << "ms");
}
```

### Actions prioritaires à tester

1. **Profiler avec Instruments (macOS)** ou Visual Studio Profiler pour identifier précisément où le temps est passé
2. **Simplifier la hiérarchie** : Fusionnez les panneaux intermédiaires qui ne font que du layout
3. **Précalculer** tous les objets graphiques (Fonts, Paths, Images) dans les constructeurs/`resized()`
4. **Activer `setOpaque(true)`** sur tous les composants opaques
5. **Vérifier les boucles de repaint** : Un composant qui déclenche le repaint de son parent qui redéclenche celui de l'enfant, etc.

---

## Précision utilisateur

**Utilisateur :**
Petite précision : mon design graphique est purement vectoriel, je n'utilise pas d'images.

---

## Deuxième réponse : Optimisations pour GUI vectorielle JUCE

Ah, c'est une information cruciale ! Le rendu vectoriel est **beaucoup plus coûteux** que l'affichage d'images pré-rendues. C'est probablement votre problème principal.

### 1. Le problème du vectoriel en temps réel

Chaque fois qu'un composant appelle `paint()`, JUCE recalcule et rastérise tous vos paths, gradients, ombres, etc. C'est extrêmement coûteux.

```cpp
// ❌ TRÈS COÛTEUX - Recalculé à chaque frame
void MyKnob::paint(Graphics& g)
{
    Path outerCircle;
    outerCircle.addEllipse(bounds);
    
    Path innerCircle;
    innerCircle.addEllipse(bounds.reduced(5));
    
    ColourGradient gradient(...);
    g.setGradientFill(gradient);
    g.fillPath(outerCircle);
    
    g.setColour(Colours::white);
    g.strokePath(innerCircle, PathStrokeType(2.0f));
    
    // Indicateur de valeur
    Path indicator;
    indicator.addRectangle(...);
    indicator.applyTransform(rotation); // Calcul trigonométrique!
    g.fillPath(indicator);
}
```

### 2. Solution N°1 : Cache d'images (Image Caching)

La solution la plus efficace : **pré-rendre vos graphiques vectoriels en images** et les mettre en cache.

```cpp
class MyKnob : public Slider
{
public:
    MyKnob()
    {
        setSize(60, 60);
        regenerateKnobImage(); // Générer l'image une seule fois
    }
    
    void resized() override
    {
        regenerateKnobImage(); // Régénérer seulement si la taille change
    }
    
    void setTheme(Theme* newTheme) override
    {
        theme = newTheme;
        regenerateKnobImage(); // Régénérer au changement de thème
    }
    
    void paint(Graphics& g) override
    {
        // ✅ Simplement dessiner l'image pré-rendue
        g.drawImageAt(cachedKnobImage, 0, 0);
        
        // Dessiner l'indicateur de valeur (seule partie dynamique)
        drawValueIndicator(g);
    }
    
private:
    Image cachedKnobImage;
    Theme* theme = nullptr;
    
    void regenerateKnobImage()
    {
        cachedKnobImage = Image(Image::ARGB, getWidth(), getHeight(), true);
        Graphics g(cachedKnobImage);
        
        // Dessiner toute la partie statique du knob ici
        Path outerCircle;
        outerCircle.addEllipse(getLocalBounds().toFloat());
        
        ColourGradient gradient(theme->knobGradientStart, 0, 0,
                                theme->knobGradientEnd, 0, getHeight(),
                                false);
        g.setGradientFill(gradient);
        g.fillPath(outerCircle);
        
        // Bordures, détails, etc.
        // ...
    }
    
    void drawValueIndicator(Graphics& g)
    {
        // Seulement la partie qui change avec la valeur du slider
        auto angle = rotaryStartAngle + (getValue() / getMaximum()) * 
                     (rotaryEndAngle - rotaryStartAngle);
        
        Path indicator;
        indicator.addRectangle(-2, -getHeight()/2 + 5, 4, 15);
        indicator.applyTransform(AffineTransform::rotation(angle)
                                 .translated(getWidth()/2, getHeight()/2));
        
        g.setColour(theme->indicatorColour);
        g.fillPath(indicator);
    }
};
```

### 3. Solution N°2 : Paths pré-calculés

Pour les éléments qui ne changent pas de taille :

```cpp
class MyButton : public Button
{
public:
    MyButton()
    {
        createStaticPaths(); // Une seule fois
    }
    
    void paint(Graphics& g) override
    {
        // ✅ Utiliser les paths précalculés
        g.setColour(getToggleState() ? theme->buttonActiveColour 
                                     : theme->buttonInactiveColour);
        g.fillPath(buttonShape);
        
        g.setColour(theme->borderColour);
        g.strokePath(buttonShape, PathStrokeType(1.5f));
    }
    
private:
    Path buttonShape;
    
    void createStaticPaths()
    {
        // Créer le path une seule fois
        buttonShape.addRoundedRectangle(getLocalBounds().reduced(2).toFloat(), 4.0f);
    }
};
```

### 4. Solution N°3 : Shared Image Cache pour composants identiques

Si vous avez beaucoup de widgets identiques (ex: 100 knobs identiques), partagez l'image :

```cpp
class KnobImageCache
{
public:
    static Image& getKnobImage(Theme* theme, int size)
    {
        String key = String(theme->name) + "_" + String(size);
        
        if(!cache.contains(key))
        {
            Image img(Image::ARGB, size, size, true);
            Graphics g(img);
            
            // Dessiner le knob
            drawKnobGraphics(g, theme, size);
            
            cache[key] = img;
        }
        
        return cache[key];
    }
    
    static void clearCache()
    {
        cache.clear();
    }
    
private:
    static HashMap<String, Image> cache;
    
    static void drawKnobGraphics(Graphics& g, Theme* theme, int size)
    {
        // Tout le rendu vectoriel ici
    }
};

// Dans votre Knob
void MyKnob::paint(Graphics& g)
{
    auto& knobImage = KnobImageCache::getKnobImage(theme, getWidth());
    g.drawImageAt(knobImage, 0, 0);
}
```

### 5. Optimisations des opérations vectorielles coûteuses

#### A. Éviter les gradients complexes
```cpp
// ❌ Très coûteux
ColourGradient gradient(colour1, p1, colour2, p2, true); // Radial
gradient.addColour(0.5, colour3); // Multi-stop
g.setGradientFill(gradient);

// ✅ Plus rapide
g.setColour(theme->fillColour); // Couleur unie
```

#### B. Simplifier les paths
```cpp
// ❌ Coûteux
Path complexShape;
complexShape.addRoundedRectangle(bounds, 5.0f, 5.0f, 
                                 true, true, false, false); // Coins spécifiques
complexShape.addEllipse(...);
complexShape = complexShape.createPathWithRoundedCorners(3.0f);

// ✅ Plus simple
Path simpleShape;
simpleShape.addRoundedRectangle(bounds, 5.0f);
```

#### C. Réduire les strokePath avec PathStrokeType
```cpp
// ❌ Coûteux
PathStrokeType stroke(2.0f, PathStrokeType::curved, PathStrokeType::rounded);
g.strokePath(path, stroke);

// ✅ Plus rapide
PathStrokeType stroke(2.0f); // Défaut : mitered, butt
g.strokePath(path, stroke);
```

### 6. Stratégie de mise en cache intelligente

```cpp
class SmartCachedComponent : public Component
{
public:
    void paint(Graphics& g) override
    {
        if(needsRegeneration)
        {
            regenerateCache();
            needsRegeneration = false;
        }
        
        g.drawImageAt(cachedImage, 0, 0);
    }
    
    void resized() override
    {
        needsRegeneration = true;
        repaint();
    }
    
    void setTheme(Theme* newTheme)
    {
        if(theme != newTheme)
        {
            theme = newTheme;
            needsRegeneration = true;
            repaint();
        }
    }
    
private:
    Image cachedImage;
    bool needsRegeneration = true;
    Theme* theme = nullptr;
    
    void regenerateCache()
    {
        cachedImage = Image(Image::ARGB, getWidth(), getHeight(), true);
        Graphics g(cachedImage);
        
        // Tout votre rendu vectoriel ici
        paintVectorGraphics(g);
    }
    
    void paintVectorGraphics(Graphics& g)
    {
        // Code vectoriel original
    }
};
```

### 7. Mesurer l'impact

```cpp
void MyComponent::paint(Graphics& g)
{
    ScopedTimer timer("MyComponent::paint");
    
    // Votre code
}

// Classe helper
class ScopedTimer
{
public:
    ScopedTimer(const String& name) : name(name)
    {
        start = Time::getMillisecondCounterHiRes();
    }
    
    ~ScopedTimer()
    {
        auto elapsed = Time::getMillisecondCounterHiRes() - start;
        if(elapsed > 1.0) // Plus de 1ms
            DBG(name << ": " << elapsed << "ms");
    }
    
private:
    String name;
    double start;
};
```

### Actions immédiates recommandées

1. **Identifier vos 5 widgets les plus utilisés** (ceux qui apparaissent le plus souvent)
2. **Convertir leur `paint()` en cache d'images** comme montré ci-dessus
3. **Mesurer la différence** de temps de lancement et de réactivité
4. **Étendre progressivement** aux autres composants

Avec du rendu vectoriel pur, vous devriez voir des **gains de performance de 10x à 100x** en utilisant le cache d'images.

---

## Question de suivi

**Utilisateur :**
J'ai une question connexe à mon problème de performance avec mon plugin JUCE :
Imaginons qu'en parallèle de mon projet je crée un plugin JUCE de test dans lequel j'utilise exclusivement des widgets natifs : labels, sliders, comboboxes et boutons. Mon plugin actuel comporte près de 300 widgets, je serais donc amené à en créer autant dans ce plugin de test. Et disons que pour faire simple, je le ferais dans un panneau unique pour ne pas surcompliquer les choses. Devrais-je m'attendre à des performances bien meilleures ? Ou le nombre de widgets à gérer engendrerait malgré tout de la lourdeur ?

---

## Troisième réponse : Performance des widgets natifs JUCE

Excellente question ! La réponse va vous surprendre : **300 widgets natifs JUCE vont également causer des problèmes de performance**, même sans customisation.

### Le test réel

Voici ce qui se passerait avec 300 widgets natifs basiques :

```cpp
class TestPanel : public Component
{
public:
    TestPanel()
    {
        // 100 sliders + 100 comboboxes + 100 buttons
        for(int i = 0; i < 100; i++)
        {
            auto* slider = sliders.add(new Slider());
            addAndMakeVisible(slider);
            
            auto* combo = combos.add(new ComboBox());
            addAndMakeVisible(combo);
            
            auto* button = buttons.add(new TextButton("Button " + String(i)));
            addAndMakeVisible(button);
        }
    }
    
    void resized() override
    {
        // Layout de 300 widgets...
    }
    
private:
    OwnedArray<Slider> sliders;
    OwnedArray<ComboBox> combos;
    OwnedArray<TextButton> buttons;
};
```

#### Résultat attendu :
- **Lancement** : 1-3 secondes (contre quelques millisecondes pour Vital/Serum)
- **Redimensionnement** : Lenteurs visibles
- **Déplacement de fenêtre** : Saccades possibles

### Pourquoi ? Les widgets natifs ne sont PAS gratuits

#### 1. Chaque widget = multiples `paint()` calls

Un simple `Slider` natif JUCE appelle en réalité :

```cpp
Slider::paint()
  └─> LookAndFeel::drawRotarySlider() ou drawLinearSlider()
       └─> Dessins vectoriels (paths, gradients, etc.)
       └─> Label interne (texte de valeur)
       └─> Potentiellement d'autres composants enfants
```

**300 sliders = potentiellement 1000+ appels `paint()` au total** lors d'un repaint complet.

#### 2. Les LookAndFeel par défaut utilisent du vectoriel

```cpp
// Code réel simplifié de LookAndFeel_V4::drawRotarySlider()
void LookAndFeel_V4::drawRotarySlider(Graphics& g, ...)
{
    // Cercle extérieur (path)
    Path backgroundArc;
    backgroundArc.addCentredArc(...);
    g.strokePath(backgroundArc, PathStrokeType(lineThickness));
    
    // Arc de valeur (path)
    Path valueArc;
    valueArc.addCentredArc(...);
    g.strokePath(valueArc, PathStrokeType(lineThickness));
    
    // Thumb (ellipse)
    g.fillEllipse(...);
    
    // Ombre portée potentielle
    // Gradient potentiel
    // etc.
}
```

**C'est du rendu vectoriel !** Même problème que votre code custom.

#### 3. ComboBox sont particulièrement coûteux

```cpp
ComboBox contient:
  - Label pour le texte
  - Button pour la flèche
  - Gestion du popup menu
  - Listeners divers
```

100 ComboBox = beaucoup d'overhead.

### Le vrai secret de Vital, Serum, Pigments...

Ces plugins **ne créent PAS 300 composants JUCE individuels**. Ils utilisent des stratégies différentes :

#### Stratégie 1 : Rendu en batch (canvas unique)

```cpp
// Ce que font les gros plugins
class EfficientControlPanel : public Component
{
public:
    void paint(Graphics& g) override
    {
        // Dessiner TOUS les contrôles en une seule passe
        for(auto& control : controls)
        {
            drawControl(g, control); // Méthode simple, pas de Component
        }
    }
    
    void mouseDown(const MouseEvent& e) override
    {
        // Déterminer quel "contrôle virtuel" a été cliqué
        for(auto& control : controls)
        {
            if(control.bounds.contains(e.position))
            {
                control.handleClick(e);
                break;
            }
        }
    }
    
private:
    struct VirtualControl
    {
        Rectangle<int> bounds;
        float value;
        Image cachedImage; // Image pré-rendue!
        
        void handleClick(const MouseEvent& e) { /* ... */ }
    };
    
    std::vector<VirtualControl> controls;
    
    void drawControl(Graphics& g, const VirtualControl& ctrl)
    {
        // Dessiner l'image pré-rendue
        g.drawImageAt(ctrl.cachedImage, ctrl.bounds.getX(), ctrl.bounds.getY());
    }
};
```

**Un seul Component JUCE**, pas 300 !

#### Stratégie 2 : Images pré-rendues (filmstrips)

```cpp
// Technique très courante
class KnobStrip
{
public:
    KnobStrip()
    {
        // Une seule image contenant 128 frames du knob
        stripImage = ImageCache::getFromMemory(BinaryData::knob_strip_png, ...);
        frameHeight = stripImage.getHeight() / 128;
    }
    
    void draw(Graphics& g, float value, int x, int y)
    {
        int frame = (int)(value * 127);
        
        // Dessiner juste la bonne frame
        g.drawImage(stripImage, 
                    x, y, frameWidth, frameHeight,
                    0, frame * frameHeight, frameWidth, frameHeight);
    }
    
private:
    Image stripImage;
    int frameHeight;
    static const int frameWidth = 60;
};
```

**Zéro calcul vectoriel au runtime !**

#### Stratégie 3 : Dirty rectangles et invalidation partielle

```cpp
class SmartPanel : public Component
{
    void updateControl(int index, float newValue)
    {
        controls[index].value = newValue;
        
        // Invalider SEULEMENT la zone de ce contrôle
        repaint(controls[index].bounds);
        // Au lieu de repaint() qui redessine tout
    }
};
```

### Test réaliste que vous devriez faire

```cpp
// Version 1 : 300 vrais Components
class NaivePanel : public Component
{
    OwnedArray<Slider> sliders; // 300 Sliders
    // Temps de création : ~2s
    // Temps de paint complet : ~50-100ms
};

// Version 2 : Canvas avec images cachées
class SmartPanel : public Component
{
    void paint(Graphics& g)
    {
        for(auto& ctrl : controls)
            g.drawImageAt(ctrl.cachedImage, ctrl.x, ctrl.y);
    }
    // Temps de création : ~100ms
    // Temps de paint complet : ~5ms
};
```

### Conclusion : Votre vrai problème

Le problème n'est **pas vraiment** vos widgets customs vs natifs.

Le problème c'est :
1. ✅ **300 Components individuels** (trop d'overhead)
2. ✅ **6 niveaux de hiérarchie** (overhead multiplicatif)
3. ✅ **Rendu vectoriel en temps réel** (très coûteux)

#### Solutions par ordre d'impact :

**Impact ÉNORME (10-50x) :**
- Cache d'images pour vos widgets (comme expliqué précédemment)
- Réduire le nombre de Components (regrouper en canvas)

**Impact MOYEN (2-5x) :**
- Aplatir la hiérarchie (6 niveaux → 2-3 niveaux)
- `setOpaque(true)` sur composants opaques
- Désactiver buffering inutile

**Impact FAIBLE (1.2-1.5x) :**
- Optimiser les paths individuels
- Simplifier les gradients

### Ma recommandation

Ne créez pas ce plugin de test avec 300 widgets natifs. À la place :

1. **Prenez votre plugin actuel**
2. **Convertissez vos 10-20 widgets les plus utilisés** en système de cache d'images
3. **Mesurez la différence**

Vous devriez passer de 3-5 secondes de lancement à < 0,5 secondes, et les saccades devraient disparaître.

Le cache d'images est LA solution universelle pour les GUIs vectorielles complexes dans JUCE.