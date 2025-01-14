# tp6_serveur_client_bdd
groupe: Sayf Adam Huseyin Ali Beritan
Voici la version mise à jour du `README.md` sans la section des auteurs :

---

# Projet : Application Client-Serveur TCP avec Base de Données MySQL

## Description du projet

Ce projet implémente une application client-serveur TCP basée sur Qt. Le serveur gère les connexions TCP des clients, communique avec une base de données MySQL, et permet aux clients d'envoyer des données pour les insérer ou les mettre à jour dans une table MySQL.  

Le client permet aux utilisateurs de se connecter au serveur, d'envoyer des messages, et de recevoir des réponses.

## Structure du projet

### Arborescence des fichiers
- **Client TCP**
  - `Client_TCP.pro` : Fichier de configuration pour construire l'application client.
  - `main.cpp` : Point d'entrée de l'application client.
  - `client.h` et `client.cpp` : Contient l'implémentation de la logique client.
  
- **Serveur TCP**
  - `serveur.pro` : Fichier de configuration pour construire l'application serveur.
  - `main.cpp` : Point d'entrée de l'application serveur.
  - `serveur.h` et `serveur.cpp` : Contient l'implémentation de la logique serveur.
  
- **Base de Données**
  - `jeu.sql` : Script SQL pour créer la base de données et la table `jeu`.

## Configuration et Installation

### Prérequis
- **Qt Framework** (version 6 ou plus récente)
- **MySQL Server**
- Outils de développement (compilateur compatible C++17)

### Étapes pour exécuter le projet
1. **Configurer la base de données** :
   - Lancer MySQL et exécuter le fichier `jeu.sql` pour créer la base de données et la table `jeu` :
     ```bash
     mysql -u root -p < jeu.sql
     ```
   - Accorder les permissions nécessaires à l'utilisateur de la base de données :
     ```sql
     GRANT ALL PRIVILEGES ON jeu.* TO 'etudiant'@'%' IDENTIFIED BY 'Passciel2';
     FLUSH PRIVILEGES;
     ```

2. **Configurer MySQL pour autoriser les connexions distantes** :
   - Modifier le fichier de configuration MySQL pour écouter sur toutes les interfaces réseau :
     ```bash
     sudo nano /etc/mysql/my.cnf
     ```
   - Localiser la ligne `bind-address` et la modifier comme suit :
     ```
     bind-address = 0.0.0.0
     ```
   - Redémarrer le service MySQL pour appliquer les modifications :
     ```bash
     sudo systemctl restart mysql
     ```

3. **Configurer le pare-feu pour autoriser le port MySQL** :
   - Ajouter une règle pour autoriser les connexions sur le port 3306 :
     ```bash
     sudo ufw allow 3306/tcp
     ```

4. **Construire et exécuter les applications** :
   - Ouvrir les projets dans Qt Creator et les construire.
   - Lancer d'abord le serveur, puis le client.

## Utilisation

### Serveur
1. Lancer le serveur.
2. Cliquer sur "Démarrer le serveur" pour écouter les connexions entrantes sur le port 2222.
3. Sur connexion d'un client, le serveur affiche les messages reçus dans une interface graphique.

### Client
1. Lancer le client.
2. Entrer l'adresse IP et le port du serveur (par défaut : `localhost` et `2222`).
3. Cliquer sur "Se connecter" pour établir une connexion.
4. Envoyer des messages au serveur via l'interface.

## Problèmes rencontrés

1. **Problème d'accès à la base de données** :
   - La base de données n'autorisait pas les connexions externes. 
   - **Solution** :
     - Accorder les droits nécessaires avec :
       ```sql
       GRANT ALL PRIVILEGES ON jeu.* TO 'etudiant'@'%' IDENTIFIED BY 'Passciel2';
       FLUSH PRIVILEGES;
       ```
     - Configurer MySQL pour écouter sur toutes les adresses IP en modifiant `bind-address` dans `/etc/mysql/my.cnf`.

2. **Problème de pare-feu** :
   - Le port 3306 utilisé par MySQL était bloqué par défaut.
   - **Solution** :
     - Ouvrir le port avec la commande :
       ```bash
       sudo ufw allow 3306/tcp
       ```

3. **Problème de connexion des autres ordinateurs au serveur MySQL** :
   - Par défaut, MySQL n'écoutait que sur `127.0.0.1`.
   - **Solution** :
     - Modifier la configuration de MySQL pour qu'il écoute sur `0.0.0.0` :
       ```bash
       sudo nano /etc/mysql/my.cnf
       ```
       Modifier :
       ```
       bind-address = 0.0.0.0
       ```
     - Redémarrer MySQL avec :
       ```bash
       sudo systemctl restart mysql
       ```

## Fonctionnalités

1. **Serveur** :
   - Démarrage et arrêt du serveur TCP.
   - Communication avec MySQL pour insérer ou mettre à jour des données.
   - Gestion des commandes spécifiques envoyées par les clients.

2. **Client** :
   - Connexion au serveur TCP.
   - Envoi et réception de messages.
   - Interface simple pour configurer le serveur cible.

## Exemple de commande client
Pour insérer ou mettre à jour des données dans la table `jeu`, le client doit envoyer une commande sous le format suivant :
```
#<ID>;<Nom>;<Club>;<Note>
```
Exemple :
```
#1;John;Real Madrid;5
```
## Ajouter une ligne dans la BDD :
```
#<17>;<Nom>;<Club>;<Note>
Pour ajouter une nouvelle ligne dans la BDD, il faut juste changer l' ID, si l'ID n'existe pas il ajoute sinon il update juste.
```
---
