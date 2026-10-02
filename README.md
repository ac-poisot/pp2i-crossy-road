# Projet PPII — Semestre S6

## Table des matières
1. [Introduction](#introduction)
2. [Membres du groupe](#membres-du-groupe)
3. [Description du projet](#description-du-projet)
4. [Prérequis](#prérequis)
5. [Installation](#installation)
6. [Exécution](#exécution)
7. [Quelques images du jeu](#quelques-images-du-jeu)

---

## Introduction

Bienvenue dans le projet **PPII — Semestre S6**. Ce projet est réalisé dans le cadre du semestre 6 pour l’unité PPII à TELECOM Nancy. Vous trouverez ici toutes les informations nécessaires pour comprendre, installer, et exécuter le projet.


## Membres du groupe

- **Poisot Anne-Cécile** — [anne-cecile.poisot@telecomnancy.eu](mailto:anne-cecile.poisot@telecomnancy.net)
- **Loisil Tom** — [tom.loisil@telecomnancy.eu](mailto:tom.loisil@telecomnancy.eu)
- **Estivals Raphaël** — [raphael.estivals@telecomnancy.eu](mailto:raphael.estivals@telecomnancy.eu)
- **Bui Kévin** — [kevin.bui@telecomnancy.eu](mailto:kevin.bui@telecomnancy.eu)


## Description du projet

- **Sujet** : Crossy Road
- **Objectifs** : Réalisation d’un Crossy Road avec interface graphique
- **Technologies utilisées** : C, SDL2

Ce projet a été fait sans l'aide de l'intelligence artificielle.


## Prérequis

- **Langages :** C (compilé avec clang)
- **Frameworks :** SDL2
- **Dépendances :** Voir [Installation](#installation)


## Installation

## Étapes générales :

1. Clonez ce dépôt :
   ```bash
   git clone https://github.com/ac-poisot/pp2i-crossy-road
   cd pp2i-crossy-road
   ```

2. Installez les dépendances :
    ```bash
    sudo apt-get install libncurses5-dev libncursesw5-dev
    sudo apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-image-dev
    sudo apt-get install libsdl2-ttf-dev
    sudo apt install libsdl2-mixer-dev
    ```

## Lancer le jeu

### Version terminal
Execute the ``run_cli.sh`` file.
```
./run_cli.sh
```


### Version graphique
Execute the ``run_gui.sh`` file.
```
./run_gui.sh
```

## Quelques images du jeu

Voici la page d'accueil du jeu.
![écran d'accueil du jeu](/pictures/cr_welcome.png)

Voici une partie en cours pour un seul joueur.
![example d'une partie en cours](/pictures/cr_in_game.png)

Voici une partie en cours contre une ia.
![example d'une partie en cours](/pictures/cr_in_game_ai.png)

Vous pouvez aussi débloquer des nouveaux personnages.
![example d'une partie en cours](/pictures/cr_skins.png)