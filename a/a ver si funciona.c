#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  Cosas que hacer... o pensar:
 *
 *  Escoger un ID para los elementos, querríamos hacerlo como si fueran
 *  números? tipo Pyro = 1000, Hydro = 1001, Cryo = 1002 y asi 
 *  sucesivamente?
 * 
 * Lo mejor puede ser que no incluyamos todos los elementos
 * concentrarse en sacar 4 personajes y pulir los porcentajes.
 * Con los personajes, deberíamos hacer algo similar como lo
 * propuesto con los elementos? tipo xiangling = 0001 ,
 * xingqiu = 0002 y asi? igual podrian ser otros nombres
 * acortados.
 *  
 * Hay que ver como inlcuimos las unidades de calibres elementales
 * en todo esto. Puede que haya mucha mierda funcionando al
 * mismo tiempo con todo esto.
 * 
 * Podríamos crear un tipo de dato ElementalAura
 * que contenga el ID del elemento y el
 * valor de las unidades de calibre actuales
 * e insertar esto en los personajes y los enemigos
 * 
 * Estaba pensando en que los 4 personajes que uno manejara
 * estuvieran en una lista y que al meter un input que se
 * "navegara" entre los personajes en pelea
 * 
 * con los enemigos podríamos hacer lo mismo?????
 * puede que no
 * 
 * Por el momento agregaría a Xingqiu y a Xiangling, 
 * empezaría a asegurarme que el combate funcione 
 * creando un monigote para testear. Nada muy espectacular.
 * Solo ataques básicos, si funciona, ver con un elemento,
 * y luego ya ver con las reacciones entre pyro e hydro
 * 
 * hay que tener en cuenta los multiplicadores de
 * los básicos y tal, balancear esto para que funcione
 * va a ser un horror.
 * 
*/

typedef struct Player
{
    Character* chr;    //lista?
    

}Player;

/*
 * Supongo que la idea sería importarlos de un
 * archivo csv
 * 
 * 
*/
typedef struct Character
{
    char ID;    //identificador
    int LVL;    //queremos niveles realmente?
    float HP;    //vida, hay que ver si queremos que sean floats o que
    float ATK;   //ataque
    float DEF;   //defensa
    int EM;    //maestria elemental
    float CritRate;  //prob critico
    float CritDMG;   //daño critico
    float EDMG;  //bonus de daño elemental
    Weapon weapon;  //arma
    void* artifacts;    //lista de artefactos
    void* abilities;    //lista de habilidades, importar csv para ello
    char elementID;  //elemento al que pertenece
    int baseHP;    //vida base
    int baseATK;   //ataque base
    int baseDEF;   //defensa base
    int baseEM;    //maestria elemental base
    float baseCritRate;  //prob critico base, es posible que no sirva
    float baseCritDMG;   //daño critico base, es posible que no sirva
    float baseEDMG;  //bonus de daño elemental base
}Character;

typedef struct Weapon
{
    void* ID; //identificador
    int LVL;    //queremos niveles realmente?
    int BaseATK;    //ataque base
    int typeStatID; //ID para identificar que tipo de substat tiene?
    float subSTAT;  //subestadística
}Weapon;

typedef struct Enemy
{
    char ID;
    int HP;    //vida
    int ATK;   //ataque
    int DEF;   //defensa
    int EM;    //maestria elemental, puede que ni lo necesite
    float CritRate;  //prob critico, puede que ni lo necesite, a no se que queramos incluir los follow ups del smt
    float CritDMG;   //daño critico, lo mismo de lo de arriba, dejar eso en un valor bajo y que hayan habilidades que suban eso?
    float EDMG;  //bonus de daño elemental, puede quedar inutilizable
}Enemy;  //será lo mismo que un personaje? R: ja, no.

typedef struct artifact
{
    char ID;    //supongo que los podríamos poner como loot o algo asi, no?
}artifact;

/*  Esta función asume que el HPBonus se entregara
 *  considerando el 100% como 1 y, por ejemplo, un
 *  50% como 0.5, o 97% como 0.97, me da miedo
 *  tener problemas de precisión, hay que probar
 * 
*/
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

/*asumiendo que el daño escalará siempre con Ataque*/
float Base_Damage (float TalentMult , float ATK)
{
    float DMG = TalentMult*ATK;
    return DMG;
}

/*  ReactBonus es un porcentaje, tratarlo como si fuera un decimal con el 100%
 *  en el 1. Añadir habilidades que suban eso? puede que un 15% como con 
 *  el crimson witch estaría bien.
 * 
 * IDElemento1 es el elemento que gatilla la reaccion.
 * IDElemento2 es el que estaba aplicado.
 * 
*/
float Amplifying_Reaction (int EM , char IDElemento1 , char IDElemento2 , float ReactBonus)
{
    float AMPReact;
    int pyro;
    int cryo;
    int hydro;

    if ((IDElemento1 == pyro && IDElemento2 == cryo) || (IDElemento1 == hydro && IDElemento2 == pyro))
    {
        AMPReact = 2 * (1 + ((2.78*EM)/(1400+EM)) + ReactBonus);
    }
    else if ((IDElemento1 == cryo && IDElemento2 == pyro) || (IDElemento1 == pyro && IDElemento2 == hydro))
    {
        AMPReact = 1.5 * (1 + ((2.78*EM)/(1400+EM)) + ReactBonus);
    }
    else
    {
        AMPReact = 1;
    }

    return AMPReact;
}
/*
 *  Lo haría solo para el swirl, ni idea si deberíamos agregar
 *  otros elementos, si es así agregaría electro, y si el enemigo
 *  tiene electro cargado en el que entonces reciba daño por
 *  turno o algo asi
 *
 * 
*/
/*
float Transformative_Reactions ()
{

}
*/

/*  Esta función asume...   MUCHAS cosas UwU
 *
 *  Puede que lo mejor sea tener algunas de las cosas calculadas
 *  de antes para no tener que estar calculandolas siempre???
 *  Querríamos calcular la AMPReact aquí dentro en vez de afuera?
 *  para eso tendríamos que incluir los elementos ahí mismo
 * 
 * EnemyResMult es la resistencia elemental del enemigo, puede
 * que querramos que anemo reduzca la resistencia y ya sin mucha
 *  
 * 
 * Es posible que el proc quede inutilizable, por... cosas
 * 
*/
float FinalDamage (float BaseDamage , int flatDMG , float DMGBonus , float Crit , 
                    float EnemyDEFMult , float EnemyResMult , float AMPReact , float TRANReac , float Proc)
{
    float DMG;
    DMG = ((BaseDamage + flatDMG)*(1+DMGBonus)*Crit*EnemyDEFMult*EnemyResMult*AMPReact)+TRANReac+Proc;
    return DMG;
}

int main()
{
    printf("uwu");
    return 0;
}