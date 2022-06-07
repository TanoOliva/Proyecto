#include <wchar.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "list.h"

#define NO_AURA 0
#define PYRO 6000
#define ELECTRO 6010
#define CRYO 6020
#define HYDRO 6030
#define AURA_TAX 0.8
#define WEAK_AMP_TRIGGER 0.5
#define STRONG_AMP_TRIGGER 2

typedef struct Player
{
    //Character* chr;    //lista?
    List * characters;
    int turns;
    int AliveCharacters;

}Player;

/*
typedef struct Weapon
{
    void* ID; //identificador
    int LVL;    //queremos niveles realmente?
    int BaseATK;    //ataque base
    int typeStatID; //ID para identificar que tipo de substat tiene?
    float subSTAT;  //subestadística
}Weapon;
*/

/*
 * U: original notation
 * GU: Elemental Gauge of source
 * GUs cannot go below zero
 * 
 * When an elemental ability is used against an enemy the resulting elemental
 * aura is:
 * 
 * xU * 0.8 = totalGU
 * 
 * Unit modifiers to gauge consuption:
 * 
 * Melt and Vaporize:
 * Weak amping elemental triggers have a 0.5x modifier.
 * Example:An enemy affected by Amber’s Charged Shot has 1.6B Pyro.
 * Using Kaeya’s E (2B Cryo) only removes 1GU Pyro because weak 
 * melt occurs when the trigger is Cryo.
 * 
 * Strong amping elemental triggers have a 2x modifier.
 * 
 * Example: An enemy affected by Kaeya’s E has 1.6B Cryo.
 * Using Diluc’s E (1A) removes 2GU worth of Cyro aura because strong 
 * melt occurs when the trigger is Pyro. This leaves us 
 * with 0GU Cryo as gauges cannot go below zero.
 * 
*/
typedef struct Elemental_Aura
{
    int ID;     //identificador
    float U;    //notación original
    float GU;    //notación de reacción
    //float U;    //Ratio de decaída: antes de esto veamos como le va a lo otro
    //int TypeOfDecay   //igual asi?
}Elemental_Aura;

typedef struct Weapon_Test
{
    void* ID; //identificador
    int LVL;    //queremos niveles realmente?
    int BaseATK;    //ataque base
    int typeStatID; //ID para identificar que tipo de substat tiene?
    float subSTAT;  //subestadística
}Weapon_Test;

typedef struct Objective_Lists
{
    List * objectivesSack1;
    size_t quantitySack1;   /*equivale a la cantidad de miembros en el equipo*/
    List * objectivesSack2;
    size_t quantitySack2;   /*equivale a la cantidad de miembros en el equipo*/
}Objective_Lists;

typedef struct Enemy_Test
{
    int ID;
    int LVL;
    float HP;    //vida
    int ATK;   //ataque
    //int DEF;   //defensa: por fórmula creo que no es necesaria
    int turns;
    List * Abilities;
    Elemental_Aura * AuraApplied;
    Objective_Lists * objectives;
}Enemy_Test;  //será lo mismo que un personaje? R: ja, no.

typedef struct ability
{
    //char * nombre;          /*para mostrar en el menú de selección?*/
    bool active;          /*checkea si la habilidad esta activa*/
    float Multiplier;    //supongo que los podríamos poner como loot o algo asi, no?
    int CDTurns;        //???????????????????????????????????????????
    int repetitions;        //Cantidad de veces que la habilidad hará efecto
    int repetitionCounter;
    int TurnsUntilEffect;   //Cantidad de turnos que le tomará para hacer efecto de nuevo
    int turnCounter;
    Elemental_Aura * aura;
}ability;

typedef struct Character_Test
{
    int ID;    //identificador, debería ser constante o no es así como funcionan?
    int LVL;    //queremos niveles realmente?
    float HP;    //vida, hay que ver si queremos que sean floats o que
    float FinalHP;
    float ATK;   //ataque
    float FinalATK;
    float DEF;   //defensa
    int EM;    //maestria elemental
    //Weapon_Test weapon;  //arma
    int elementID;  //elemento al que pertenece, debería ser constante o no es así como funcionan?
    List * Abilities;
    Elemental_Aura * AuraApplied;  //puede que sea mejor para las habilidades
}Character_Test;

/*igual se deberia cambiar a habilidades activas?*/
typedef struct action
{
    Character_Test * character;
    ability * abilityUsed;
}action;



const char *get_csv_field (char * tmp, int k)
{
    int open_mark = 0;
    char* ret = (char*) malloc (100*sizeof(char));
    int ini_i = 0, i = 0;
    int j = 0;
    while(tmp[i+1] != '\0'){

        if(tmp[i] == '\"')
        {
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ',')
        {
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ',')
        {
            if(k==j)
            {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j)
    {
       ret[i-ini_i] = 0;
       return ret;
    }

    return NULL;
}

/*Puede que haya que quitarle el while, de todas formas el tema de seleccion de personajes hay que echarle un ojo*/
char * seleccionDePersonajes()
{
    int opcion;
    int cont = 0;
    while (cont < 1)
    {
        printf("Opciones:\n");
        printf("1)Xiangling\n2)Xingqiu\n");
        printf("Seleccione los miembros del equipo: ");
        scanf("%i", &opcion);

        switch (opcion)
        {
        case 1: ;
            return "Xiangling.csv";
            break;

        case 2: ;
            return "Xingqiu.csv";
            break;

        case 3: ;
            return "Bennett.csv";
            break;
        
        
        default:
            break;
        }

        cont += 1;
    }
}


/*
 * Los personajes se importarán desde un csv que tenga
 * sus datos
 * 
 * Las habilidades estarán en un csv aparte... creo
 *
 * Cuidado para no cagarla con punteros, lo que daba error se ha comentado,
 * hay que adaptar la funcion para los campos que queremos llenar
 * 
 * Las habilidades quedan en el siguiente orden:
 * Q - E
 * 
*/
List * Import_CharacterArchive ()
{
    //FILE *fp = fopen ("csv juego UwU.csv", "r");
    char * seleccionado;
    char linea[1024];
    int campo;
    int characterCounter = 0; //posible inutil
    int cont = 0;
    
    List * CharacterList = createList();
            
    //listaGeneral->canciones = createList();
    //listaGeneral->capacidad = 1;
    while (characterCounter < 4)
    {
        /*Menú de seleccion*/
        /*podría ser una funcion de menu que retorne un valor que decidira que se mete o algo asi supongo no?*/
        seleccionado = seleccionDePersonajes();

        FILE *fp = fopen (seleccionado, "r");
        cont = 1;

        /*Se reserva memoria*/
        Character_Test * character = (Character_Test *) malloc (sizeof(Character_Test));
        character->AuraApplied = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
        character->Abilities = createList();

        ability * Eability = (ability *) malloc (sizeof(ability));
        Eability->aura = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
        ability * Qability = (ability *) malloc (sizeof(ability));
        Qability->aura = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
        
        while (fgets (linea, 1024, fp) != NULL)
        {
            if (cont == 1)
            {   
                for (campo = 0; campo < 7; campo += 1)
                {
                    char *aux = (char *) get_csv_field(linea , campo);
                    if (campo == 0)
                    {
                        //ID DEL PERSONAJE
                        character->ID = atoi(aux);
                        printf("se ha insertado ID: %i\n", character->ID); 
                    }
                    if (campo == 1)
                    {
                        // NIVEL
                        character->LVL = atoi(aux);
                        printf("se ha insertado LVL: %i\n", character->LVL); 
                    }
                    if (campo == 2)
                    {
                        // PUNTOS DE VIDA
                        character->HP = atoi(aux);
                        printf("se ha insertado la vida: %f\n", character->HP); 
                    }
                    if (campo == 3)
                    {
                        //ATAQUE
                        character->ATK = atoi(aux);
                        printf("se ha insertado ATK: %f\n", character->ATK); 
                    }
                    if (campo == 4)
                    {
                        //DEFENSA
                        character->DEF = atoi(aux);
                        printf("se ha insertado DEF: %f\n", character->DEF); 
                    }
                    if (campo == 5)
                    {
                        //MAESTRÍA ELEMENTAL
                        character->EM = atoi(aux);
                        printf("se ha insertado EM: %i\n", character->EM); 
                    }
                    if (campo == 6)
                    {
                        /*ID DEL ELEMENTO*/
                        character->elementID = atoi(aux);
                        printf("se ha insertado el Element ID: %i\n\n", character->elementID); 
                    }
                }

                /*no se puede calcular ataque final aca porque aun no tiene 
                  arma ni tampoco el orden de las funciones lo permite*/
            }
            else if (cont == 2)     /*Se llena la abilidad de la E*/
            {
                for (campo = 0; campo < 10; campo += 1)
                {
                    char *aux = (char *) get_csv_field(linea , campo);
                    if (campo == 0)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->active = atoi(aux);
                        printf("El estado de la habilidad E es: %i\n", Eability->active);
                    }
                    if (campo == 1)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->Multiplier = atoi(aux);
                        printf("El multiplicador de la habilidad E es: %f\n", Eability->Multiplier);
                    }
                    if (campo == 2)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->CDTurns = atoi(aux);
                        printf("Los turnos de CD son: %i\n", Eability->CDTurns);
                    }
                    if (campo == 3)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->repetitions = atoi(aux);
                        printf("Las repeticiones que tendra la habilidad son: %i\n", Eability->repetitions);
                    }
                    if (campo == 4)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->repetitionCounter = atoi(aux);
                        printf("El contador de repeticiones esta en: %i\n", Eability->repetitionCounter);
                    }
                    if (campo == 5)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->TurnsUntilEffect = atoi(aux);
                        printf("La habilidad hara efecto cada %i turnos\n", Eability->TurnsUntilEffect);
                    }
                    if (campo == 6)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Eability->turnCounter = atoi(aux);
                        printf("El contador de turnos de la habilidad esta en: %i\n", Eability->turnCounter);
                    }
                    if (campo == 7)
                    {
                        Eability->aura->ID = atoi(aux);     //se podría usar el ID del elemento del personaje pero que paja
                        printf("el ID elemental de la habilidad E es: %i\n", Eability->aura->ID);
                    }
                    if (campo == 8)
                    {
                        Eability->aura->U = atoi(aux);
                        printf("los U de la habilidad E es: %f\n", Eability->aura->U);
                    }
                    if (campo == 9)
                    {
                        Eability->aura->GU = atoi(aux);
                        printf("los GU de la habilidad E es: %f\n\n", Eability->aura->GU);
                    }
                }
            }
            else if (cont == 3)     /*Se llena la abilidad de la Q*/
            {
                for (campo = 0; campo < 10; campo += 1)
                {
                    char *aux = (char *) get_csv_field(linea , campo);
                    if (campo == 0)
                    {
                        /*ESTADO DE LA HABILIDAD*/
                        Qability->active = atoi(aux);
                        printf("El estado de la habilidad Q es: %i\n", Qability->active); 
                    }
                    if (campo == 1)
                    {
                        /*MULTIPLICADOR DE LA HABILIDAD*/
                        Qability->Multiplier = atoi(aux);
                        printf("El multiplicador de la habilidad Q es: %f\n", Qability->Multiplier); 
                    }
                    if (campo == 2)
                    {
                        //Muy probablemente no necesitemos esto y lo cambiemos por otra cosa, pero dejemos el campo asi por ahora
                        /*TURNOS DE CD DE LA HABILIDAD*/
                        Qability->CDTurns = atoi(aux);
                        printf("Los turnos de CD son: %i\n", Qability->CDTurns); 
                    }
                    if (campo == 3)
                    {
                        /*CANTIDAD DE REPETICIONES QUE TENDRA LA HABILIDAD*/
                        Qability->repetitions = atoi(aux);
                        printf("Las repeticiones que tendra la habilidad son: %i\n", Qability->repetitions); 
                    }
                    if (campo == 4)
                    {
                        /*CONTADOR DE REPETICIONES HASTA QUE LA HABILIDAD TERMINE*/
                        Qability->repetitionCounter = atoi(aux);
                        printf("El contador de repeticiones esta en: %i\n", Qability->repetitionCounter); 
                    }
                    if (campo == 5)
                    {
                        /*CANTIDAD DE TURNOS QUE LE TOMARÁ PARA HACER EFECTO DE NUEVO*/
                        Qability->TurnsUntilEffect = atoi(aux);
                        printf("La habilidad hara efecto cada %i turnos\n", Qability->TurnsUntilEffect); 
                    }
                    if (campo == 6)
                    {
                        /*CONTADOR DE TURNOS PARA QUE HAGA EFECTO*/
                        Qability->turnCounter = atoi(aux);
                        printf("El contador de turnos de la habilidad esta en: %i\n", Qability->turnCounter); 
                    }
                    if (campo == 7)
                    {
                        Qability->aura->ID = atoi(aux);     //se podría usar el ID del elemento del personaje pero que paja
                        printf("el ID elemental de la habilidad Q es: %i\n", Qability->aura->ID);
                    }
                    if (campo == 8)
                    {
                        Qability->aura->U = atoi(aux);
                        printf("los U de la habilidad Q es: %f\n", Qability->aura->U);
                    }
                    if (campo == 9)
                    {
                        Qability->aura->GU = atoi(aux);
                        printf("los GU de la habilidad Q es: %f\n\n", Qability->aura->GU);
                    }
                }
            }
            cont += 1;
            //listaGeneral->capacidad += 1;
        }

        character->AuraApplied->ID = 0;
        character->AuraApplied->U = 0;
        character->AuraApplied->GU = 0;

        pushFront(character->Abilities , Eability);
        pushFront(character->Abilities , Qability);

        pushFront(CharacterList , character);

        characterCounter += 1;
        if (characterCounter == 2)
        {
            break;
        }
    }

    printf("xiangling importada\n");
    printf("xingqiu importado\n");
    return CharacterList;  //cuidado con los punteros
}


float Final_HP (int BaseHP , float HPBonus , int flatHPBonus)
{
    float HP = (BaseHP*(1+HPBonus))*flatHPBonus;
    return HP;
}

float Final_ATK (int BaseATK , float ATKBonus , int flatATKBonus , int WeaponATK)
{
    float ATK = ((BaseATK+WeaponATK)*(1+ATKBonus))+flatATKBonus;
    return ATK;
}

float Final_DEF (int BaseDEF , float DEFBonus , int flatDEFBonus)
{
    float DEF = (BaseDEF*(1+DEFBonus))*flatDEFBonus;
    return DEF;
}


float Base_Damage (float Talent , float ATK)
{
    return (Talent*0.01)*ATK;
}


float Enemy_Defense_Multiplier (int CharacterLevel , int EnemyLevel , float DefenseReduction)
{
    float EnemyDefMult;
    EnemyDefMult = ((CharacterLevel + 100)/((CharacterLevel + 100) + (EnemyLevel + 100)*(1 - DefenseReduction)));
    return EnemyDefMult;
}

void Aura_Application (Elemental_Aura * EnemyAura , Elemental_Aura * AbilityAura)
{
    EnemyAura->ID = AbilityAura->ID;
    EnemyAura->U = AbilityAura->U;      //es igual al de la habilidad para el decay,
    EnemyAura->GU = AbilityAura->GU * AURA_TAX;
}

void Weak_Amping_Trigger (Elemental_Aura * EnemyAura , Elemental_Aura * AbilityAura)
{
    EnemyAura->GU -= AbilityAura->GU * WEAK_AMP_TRIGGER;
    if (EnemyAura->GU <= 0)
    {
        EnemyAura->ID = NO_AURA;
        EnemyAura->U = 0;
        EnemyAura->GU = 0;
    }
}

void Strong_Amping_Trigger (Elemental_Aura * EnemyAura , Elemental_Aura * AbilityAura)
{
    EnemyAura->GU -= AbilityAura->GU * STRONG_AMP_TRIGGER;
    if (EnemyAura->GU <= 0)
    {
        EnemyAura->ID = NO_AURA;
        EnemyAura->U = 0;
        EnemyAura->GU = 0;
    }
}

/* Notas:
 * 1.- ReactionBonus removido porque que paja
 *
 *
 * 
 * 
*/
float Amplifying_Reaction (Elemental_Aura * EnemyAura , Elemental_Aura * AbilityAura , int EM)
{
    float AMPReact = 1;
    int pyro = PYRO;
    int cryo = CRYO;
    int hydro = HYDRO;

    /*Si el enemigo no tiene ningun aura aplicada, no hay reaccion y se aplica ele elemento de la habilidad sobre el enemigo*/
    if (EnemyAura->ID == 0)
    {
        Aura_Application(EnemyAura , AbilityAura);
        return AMPReact;
    }

    /*Para cuando los elementos sean iguales, puede que queramos ignorar esto*/
    /*
    if (EnemyAura->ID == AbilityAura->ID)
    {

    }
    */

    
    if ((AbilityAura->ID == pyro && EnemyAura->ID == cryo) || (AbilityAura->ID == hydro && EnemyAura->ID == pyro))
    {
        printf("Strong Amplifying reaction triggered!\n");
        Strong_Amping_Trigger(EnemyAura , AbilityAura);
        AMPReact = 2 * (1 + ((2.78*EM)/(1400+EM)));
    }
    else if ((AbilityAura->ID == cryo && EnemyAura->ID == pyro) || (AbilityAura->ID == pyro && EnemyAura->ID == hydro))
    {
        printf("Weak Amplifying reaction triggered!\n");
        Weak_Amping_Trigger(EnemyAura , AbilityAura);
        AMPReact = 1.5 * (1 + ((2.78*EM)/(1400+EM)));
    }

    return AMPReact;
}

/* Notas:
 * 1.- No hay crítico porque que paja
 * 2.- Se removió EnemyResistanceMultiplier porque que paja
 * 3.- Se remueve flatDMG porque que paja
 * 4.- Se remueve AMPReact porque se calculará dentro de la función
 * 5.- Se remueve TRANReac porque se calculará dentro de la función
 * 6.- Se remueve Proc porque que paja, ya veré como lo hacemos para eso, supongo que chequear si el enemigo
 *     está imbuido de cierta aura y calcular eso, hay que revisar como haremos lo de las procs de electro
 *     carga, porque joder vaya quebradero de cabeza será eso
 * 
*/
float FinalDamage (float BaseDamage , float DMGBonus , float EnemyDEFMult ,  Elemental_Aura * EnemyAura , 
                    Elemental_Aura * AbilityAura , int EM)
{
    //DMG = ((BaseDamage + flatDMG)*(1+DMGBonus)*Crit*EnemyDEFMult*EnemyResMult*AMPReact)+TRANReac+Proc;
    printf("BaseDamage: %f\n", BaseDamage);
    printf("DMG bonus: %f\n", DMGBonus);
    printf("EnemyDEFMul: %f\n", EnemyDEFMult);

    float AMPReact = Amplifying_Reaction (EnemyAura , AbilityAura , EM);
    //float TRANReact = ;

    float DMG;
    DMG = ((BaseDamage)*(1+DMGBonus)*EnemyDEFMult*AMPReact);
    printf("final damage: %f\n", DMG);
    return DMG;
}

/* Cambiar para que pase el dato del jugador y no solo la
 * lista. Por el tema de los personajes vivos, aunque le podrías
 * pasar un puntero tambien te digo, pero son una mierda asi
 * que no.
 * 
*/
void CharacterInsert(List * CharacterList)
{
    Character_Test * WaterBoi = (Character_Test *) malloc (sizeof(Character_Test));
    WaterBoi->ID = 4040;
    WaterBoi->LVL = 1;
    WaterBoi->HP = 6027;
    WaterBoi->ATK = 119;
    WaterBoi->DEF = 447;
    WaterBoi->EM = 0;
    WaterBoi->elementID = 6030;
    pushFront(CharacterList , WaterBoi);
    printf("se ha insertado a %i\n", WaterBoi->ID);
}

Enemy_Test * EnemyCreateTest()
{
    Enemy_Test * GINO = (Enemy_Test *) malloc(sizeof(Enemy_Test));
    GINO->objectives->objectivesSack1 = createList();
    GINO->objectives->quantitySack1 = 2;
    GINO->objectives->objectivesSack2 = createList();
    GINO->objectives->quantitySack1 = 2;
    GINO->ID = 5000;
    GINO->LVL = 1;
    GINO->HP = 44968;
    GINO->ATK = 4409;
    GINO->turns = 3;

    GINO->Abilities = createList();
    ability * Ability_1 = (ability *) malloc (sizeof(ability));
    Ability_1->aura = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
    Ability_1->active = false;
    Ability_1->Multiplier = 0.12;
    Ability_1->CDTurns = 0;
    Ability_1->repetitions = 1;
    Ability_1->repetitionCounter = 0;
    Ability_1->TurnsUntilEffect = 1;
    Ability_1->turnCounter = 0;
    Ability_1->aura->ID = 0;
    Ability_1->aura->U = 0;
    Ability_1->aura->GU = 0;
    pushFront(GINO->Abilities , Ability_1);


    ability * Ability_2 = (ability *) malloc (sizeof(ability));
    Ability_2->aura = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
    Ability_2->active = false;
    Ability_2->Multiplier = 0.1;
    Ability_2->CDTurns = 0;
    Ability_2->repetitions = 1;
    Ability_2->repetitionCounter = 0;
    Ability_2->TurnsUntilEffect = 1;
    Ability_2->turnCounter = 0;
    Ability_2->aura->ID = 0;
    Ability_2->aura->U = 0;
    Ability_2->aura->GU = 0;
    pushFront(GINO->Abilities , Ability_2);


    GINO->AuraApplied = (Elemental_Aura *) malloc (sizeof(Elemental_Aura));
    GINO->AuraApplied->ID = 0;
    GINO->AuraApplied->U = 0;
    GINO->AuraApplied->GU = 0;

    return GINO;
}


/* Empieza a mirar personaje a personaje, habilidad a habilidad
 * Selecciona un personaje, selecciona una habilidad, y mira si está activa,
 * si lo esta, le sube uno el contador de repeticiones, si este es igual a las
 * repeticiones maximas que puede tener, la habilidad se termina
 *
 * Esta funcion sufre de que las reacciones que se gatillen estan determinadas
 * no por el orden de ejecucion sino que por el orden del equipo
 * 
 * POSIBLEMENTE SE DESECHE ESTA IDEA POR OTRA COSA, VAYA PUTISIMA MIERDA POR CIERTO
 * 
*/
void Update_Abilities (List * actionList , Enemy_Test * enemy)
{
    printf("\n\n\nRevisando habilidades:\n");
    action * actualAction = firstList(actionList);

    while (actualAction != NULL)
    {
        printf("revisando accion de: %i\n", actualAction->character->ID);

        printf("%i tiene una habilidad activa\n", actualAction->character->ID);
        actualAction->abilityUsed->turnCounter += 1;
        if (actualAction->abilityUsed->turnCounter == actualAction->abilityUsed->TurnsUntilEffect)
        {
            actualAction->abilityUsed->turnCounter = 0;
            /*hace efecto en el enemigo*/
            enemy->HP -= FinalDamage(Base_Damage(actualAction->abilityUsed->Multiplier , actualAction->character->ATK) , 1 , 1 , 
                                                    enemy->AuraApplied , actualAction->abilityUsed->aura , actualAction->character->EM);
            printf("Enemy Aura Data:\nID: %i\nU: %f\nGU: %f\n", enemy->AuraApplied->ID , enemy->AuraApplied->U , enemy->AuraApplied->GU);

            actualAction->abilityUsed->repetitionCounter += 1;
            printf("la habilidad se ha usado: %i/%i\n", actualAction->abilityUsed->repetitionCounter , actualAction->abilityUsed->repetitions);
            if (actualAction->abilityUsed->repetitionCounter == actualAction->abilityUsed->repetitions)
            {
                printf("se desactiva la habilidad de %i\n", actualAction->character->ID);
                actualAction->abilityUsed->repetitionCounter = 0;
                actualAction->abilityUsed->active = false;
                popCurrent(actionList);
            }
        }

        actualAction = nextList(actionList);
    }

    printf("\n\n");
}


void fillObjectivesList(List * characterList , Objective_Lists * objectivesSack , Objective_Lists * quantitySack)
{
    Character_Test * character = firstList(characterList);
    /*escoger numero entre 1-cantPersonajes entonces iteras X-1 para meter el personaje*/
}



/* Crea y revuelve las bolsas para seleccionar a los personajes a los que el enemigo atacará
 * 
 * 
 * 
 * 
*/
void Objective_Randomizer(List * characterList , Objective_Lists * objectives)
{
    fillObjectivesList(characterList , objectives->objectivesSack1 , objectives->quantitySack1);
    fillObjectivesList(characterList , objectives->objectivesSack2 , objectives->quantitySack2);
}



/*
 * Una forma para que el enemigo decida a quien atacar podría ser plantearlo
 * de la siguiente forma:
 * Tenemos dos bolsas con los personajes, las revolvemos bien, y luego
 * sacamos a un personaje, este personaje será el objetivo de un ataque
 * y luego sacamos otro de la misma bolsa hasta que se termine. Ahora
 * pasamos a la siguiente bolsa, hasta que se termine. Cuando estas
 * estén vacías las volvemos a llenar y el ciclo vuelve a empezar.
*/
Character_Test * objectiveSelection (Enemy_Test * enemy)
{
    /*crear funcion para seleccionar un personaje aleatoriamente, hacer lo de las listas*/
    /*mientras tanto que le den por culo al primero aksdfhbgkjhshdfgkljhsdlfkjhgsjhdfkgsj*/

    return firstList(characterList);
}



void Enemy_Action(Character_Test * character , Enemy_Test * enemy)
{
    ability * SelectedAbility = firstList(enemy->Abilities);
    printf("%i's hp before attack: %f\n", character->ID , character->HP);
    character->HP -= Base_Damage(enemy->ATK , SelectedAbility->Multiplier);
    /*que siiiiii que ya se ha calculado y calcularlo de nuevo es mal, pero es para testear y ya esta, por el momento*/
    printf("Enemy has dealed %f damage\n", Base_Damage(enemy->ATK , SelectedAbility->Multiplier));

}



/*
 * Puede que querramos dejar el while como while(1) y dentro de
 * eso ponemos unos if para ver si el combate se termina o no
 *
 * Hay que ver como aplicar lo de las habilidades y los ataques del enemigo
 * 
 * Una forma para que el enemigo decida a quien atacar podría ser plantearlo
 * de la siguiente forma:
 * Tenemos dos bolsas con los personajes, las revolvemos bien, y luego
 * sacamos a un personaje, este personaje será el objetivo de un ataque
 * y luego sacamos otro de la misma bolsa hasta que se termine. Ahora
 * pasamos a la siguiente bolsa, hasta que se termine. Cuando estas
 * estén vacías las volvemos a llenar y el ciclo vuelve a empezar.
 * 
 * PODRIAMOS HACER UN ARBOL CON EL ORDEN DE LAS HABILIDADES QUE SE USARON, O SEA, PONEMOS UNA KEY QUE INDIQUE
 * EL MOMENTO, POR EJEMPLO EN EL QUE SE USO UNA HABILIDAD, UNA VEZ QUE EL EFECTO DE ESTA SE ACABE, SE ELIMINA EL NODO
 * DEL ARBOL
 * 
 * CADA NODO TENDRIA AL PERSONAJE CON SU HABILIDAD UTILIZADA
 * 
 * 
 * 
 * 
*/
void Combat (Player * player , Enemy_Test * GINO)
{
    Objective_Randomizer(player->characters , GINO->objectives);
    List * actionList = createList();
    int opcion;
    //Character_Test * selectedCharacter;
    player->AliveCharacters = 2;    //esta en 2 porque estamos probando con 2 personaje
    printf("personajes vivos : %i\n", player->AliveCharacters);
    //printf("Play Take Over\n");
    while (player->AliveCharacters == 0 || GINO->HP > 0)
    {
        Character_Test * selectedCharacter = firstList(player->characters);
        printf("Player Turn!\n");

        for (int Player_Turns = player->turns ; Player_Turns > 0 ; Player_Turns -= 1)
        {
            printf("Selected Character: %i\n", selectedCharacter->ID);
            printf("Selected Character HP: %f\n", selectedCharacter->HP);
            printf("Remaining turns: %i\n", Player_Turns);
            printf("Enemy hp: %f\n", GINO->HP);
            printf("Enemy Aura Data:\nID: %i\nU: %f\nGU: %f\n", GINO->AuraApplied->ID , GINO->AuraApplied->U , GINO->AuraApplied->GU);
            printf("Actions:\n1)Attack\n2)Use Ability\n3)Pass Turn\n0)acabar funcion\n");
            printf("Select Choice: ");
            fflush(stdin);
            scanf("%i", &opcion);
            
            printf("--------------------------\n");
            switch (opcion)
            {
            case 1: ;
                GINO->HP -= selectedCharacter->ATK;
                printf("%i ha inflingido %f de daño a Gino\n", selectedCharacter->ID , selectedCharacter->ATK);
                Update_Abilities(actionList , GINO);
                //printf("Vida restante del enemigo: %f\n", GINO->HP);
                break;

            case 2: ;
                ability * selectedAbility = (ability *) malloc(sizeof(ability));
                selectedAbility = firstList(selectedCharacter->Abilities);
                printf("Abilities:\n1)Q\n2)E\n");
                printf("Select Ability: ");
                fflush(stdin);
                scanf("%i", &opcion);
                for (size_t cont = 1 ; cont < opcion ; cont++)
                {
                    selectedAbility = nextList(selectedCharacter->Abilities);
                }

                /*probando con que solo la funcion update_abilities chequee si la habilidad debe pegar o no*/
                /*simplemente activa la habilidad sin chequear si ya esta activada o no*/
                /*podriamos hacer que se reinicie la habilidad si ya estaba activada o que no permita activarla de nuevo*/
                printf("Se ha activado la habilidad seleccionada\n");
                selectedAbility->active = true;
                selectedAbility->repetitionCounter = 0;
                selectedAbility->turnCounter = 0;
                action * actionData = (action *) malloc (sizeof(action));
                actionData->character = selectedCharacter;
                actionData->abilityUsed = selectedAbility;
                pushBack(actionList , actionData);

                //printf("El estado actual de la habilidad es: %i\n", selectedAbility->active);
                //printf("multiplicador: %f\n", selectedAbility->Multiplier);
                //GINO->HP -= FinalDamage(Base_Damage(selectedAbility->Multiplier , selectedCharacter->ATK) , 1 , 1 , GINO->AuraApplied , selectedAbility->aura , selectedCharacter->EM);
                //printf("Vida restante del enemigo: %f\n", GINO->HP);
                Update_Abilities(actionList , GINO);
                break;

            case 3: ;
                selectedCharacter = nextList(player->characters);
                if (selectedCharacter == NULL)
                {
                    selectedCharacter = firstList(player->characters);
                }
                Player_Turns += 1;
                //printf("Selected Character: %i\n", selectedCharacter->ID);
                break;

            case 0: ;
                return;
                break;
            default: ;
                break;
            }
            printf("--------------------------\n");
        }

        printf("Enemy Turn!\n");
        for (int Enemy_Turns = 0 ; Enemy_Turns <= GINO->turns ; Enemy_Turns += 1)
        {
            printf("--------------------------\n");
            //y aqui hace sus mierdas
            printf("and he doesn't just fucking dies anymore\n");
            /*decide objective*/
            Character_Test * characterObjective = objectiveSelection(GINO);
            Enemy_Action(characterObjective , GINO);
            printf("character %i has %f hp left\n", characterObjective->ID , characterObjective->HP);
            printf("gino has %f hp left\n", GINO->HP);
            printf("--------------------------\n");
            break;
        }
    }

    if (player->AliveCharacters == 0)
    {
        printf("alexa this is so sad, please play despacito\n");
        return;
    }

    if (GINO->HP == 0)
    {
        printf("alexa this is so sad, please play Japanese Goblin\n");
        return;
    }
}



int main ()
{
    Player * player = (Player *) malloc(sizeof(Player));
    List * CharacterList = createList();

    /*
    CharacterInsert(CharacterList);
    Character_Test * WaterBoi = (Character_Test *) malloc (sizeof(Character_Test));
    WaterBoi = firstList(CharacterList);
    printf("se ha ingresado: %i\n", WaterBoi->ID);
    Character_Test * xiangling = (Character_Test *) malloc (sizeof(Character_Test));
    xiangling = nextList(CharacterList);
    //Character_Test * CharacterListTest = (Character_Test *) malloc (4 * sizeof(Character_Test));
    printf("se ha ingresado: %i\n", xiangling->ID);
    //printf("se ha ingresado: %i", WaterBoi->ID);
    */

    CharacterList = Import_CharacterArchive();
    player->characters = CharacterList;
    //Import_CharacterArchive(player->characters);
    player->turns = 4;         //deberian ser como 4 o algo asi

    Enemy_Test * GINO = EnemyCreateTest();

    Combat(player , GINO);

    return 0;
}