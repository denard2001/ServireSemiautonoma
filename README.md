# Servire Semi-Autonomă realizată de un robot chelner

## Descriere
Proiectul integrează un program Arduino și o interfață Python pentru facilitarea serviciilor semi-autonome în restaurante prin intermediul unui robot chelner. Utilizatorii pot configura spațiul restaurantului prin intermediul unei interfețe intuitive, plasând elemente cum ar fi drumuri, mese și bucătăria pe o hartă virtuală.

## Clonarea acestui repository
Clonați acest repository pe mașina locală folosind comanda:

git clone https://github.com/denard2001/ServireSemiautonoma.git

## Instalare

### Dependințe
- **Python**: Descarcă și instalează de pe [site-ul oficial Python](https://www.python.org/downloads/).
- **Visual Studio Code/orice alt IDE pentru Python**: Descarcă și instalează de pe [site-ul oficial Visual Studio Code](https://code.visualstudio.com/) pentru a modifica și rula codul Python.

### Setare și Rulare
1. Se conectează robotul la laptop folosind cablul USB.
2. Identifică portul COM al plăcii Arduino căutând în *Device Manager*.
3. Deschide programul ”configurator_restaurant.py” în aplicația Visual Studio Code. Pentru acest lucru se vor urma urmatorii sub-pași:
 - 3.1. Se deschide aplicația visual Studio Code sau orice alt editor.
 - 3.2. Mergeți pe File -> Open Folder
 - 3.3. Selectați folderul unde aveți salvat codul 
 - 3.4. În partea stângă la explorer se alege fișierul ”configurator_restaurant.py”
4. Actualizează codul Python cu portul COM corespunzător. Mai exact linia:
   - ”arduino = serial.Serial(port = 'COM8', baudrate = 115200, timeout=.1)”. ( doar portul COM se actualizeaza!)
5. Se deschide un terminal în Visual Studio Code mergând în bara din partea de sus și facând click pe Terminal ->  New Terminal
6. Rulează interfața Python pentru configurarea hărții restaurantului folosind comanda:
   python configurator_restaurant.py
7. Se deschide interfața și se introduc dimensiunile restaurantului în metri. Dupa ce s-au introdus se face click pe ”Confirma”.
8. Se deschide o nouă fereastră și se amplasează fiecare element al restaurantului pe grid. Pentru a amplasa un element se selectează elemementul ( Drum, Masă, Bucătărie ) și se amplasează prin click pe poziția unde se dorește așezat pe hartă.
9. După finalizarea hărții se face click pe butonul ”Finalizare” pentru ca harta sa se încarce în memoria robotului.
10. Pentru a închide aplicația se face click pe butonul ”Ieșire”.

## Descriere
Configurează harta restaurantului prin interfața Python, plasând elementele următoare:drumul, mesele și bucătăria. Robotul chelner va naviga conform acestei hărți primind comenzi de la telecomanda.

## Contact
Pentru orice nelămurire sau problemă întâlnită puteți să ne contactați la adresa de e-mail: lazadenard@gmail.com
