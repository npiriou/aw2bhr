# Lancer Spann Island avec le modèle ML

La ROM modifiée ne doit pas être ouverte directement avec le mGBA installé dans `C:\Program Files`. Le modèle a besoin du lanceur, qui démarre automatiquement la bonne version de mGBA, le pont Lua et le processus Python.

## Procédure normale

1. Fermer toutes les fenêtres mGBA déjà ouvertes.
2. Ouvrir le dossier `X:\dev\aw2bhr-spann-local` dans l'Explorateur Windows.
3. Double-cliquer sur `START-SPANN-ML.cmd`.
4. Au premier lancement, ouvrir **Outils → Paramètres → Contrôleurs**, choisir la manette et définir les boutons. Cliquer sur **Appliquer**, puis **OK**. La configuration reste enregistrée dans cette version portable de mGBA.
5. Garder la fenêtre de commande ouverte. Elle héberge le lanceur pendant la partie.
6. Attendre que mGBA s'ouvre. Le lanceur attend d'abord que runE-U83 soit chargé, ce qui peut prendre quelques secondes.
7. Dans le jeu, choisir **War Room**, puis **Spann Island**.
8. Jouer normalement et terminer le tour humain. Au tour ennemi, runE-U83 envoie ses décisions à AW2; les déplacements, captures, constructions et changements de tour sont exécutés par le moteur original du jeu.
9. Pour arrêter, fermer mGBA. Le processus Python local est alors arrêté automatiquement.

## À ne pas faire

Ne pas double-cliquer sur `build-mod\aw2bhr-spann-local.gba`. L'association Windows ouvre actuellement mGBA 0.10.5, qui ne charge pas automatiquement le script Lua. Sur Spann Island, la ROM modifiée ne lance jamais l'IA native ennemie : si le pont ML est absent ou échoue, le tour ennemi reste arrêté afin de rendre l'erreur visible.

## En cas de problème

Le lanceur utilise la build Windows 32 bits de mGBA du commit `3a5bc24629867576b0fb576a5d5a21d3b3d6b576`. **Outils → Paramètres → Contrôleurs** a été testé sur cette build avec la 8BitDo connectée : la fenêtre reste réactive. Aucun réglage de l'autre installation de mGBA n'est copié automatiquement.

Le journal principal est `X:\dev\aw2bhr-spann-local\runtime\bridge.stderr.log`. Les lignes suivantes indiquent un démarrage correct :

```text
loading model once from ...runE-U83-inference.pt
ready: mode=model runtime=...
```

Après un tour ennemi, le même fichier doit contenir des lignes `session=... request=... action=... native=...`.

Pour afficher davantage de transitions, lancer depuis PowerShell :

```powershell
cd X:\dev\aw2bhr-spann-local
.\tools\run_spann_local.ps1 -Mode model -Trace
```
