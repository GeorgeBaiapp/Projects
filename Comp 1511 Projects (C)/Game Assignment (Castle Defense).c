// Assignment 2 20T2 COMP1511: Castle Defense
//
// This program was written by George Bai (z5254903)
// on 07/08/2020
//
// Version 1.0.0 (2020-07-20): Assignment released.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "realm.h"

typedef struct enemy *Enemy;

// These are the minimum requirements to be considered a tower
#define MIN_POWER 1
#define MIN_USES 1

// These are the minimum requirements to be considered an alive enemy
#define MIN_HP 1
///////////////////////////////////////////////////////////////////////

// `struct realm` represents a realm, which represents the state of the
// entire program. It is mainly used to point to a linked list of 
// locations, though you may want to add other fields to it.
//
// You may need to add fields to struct realm.
struct realm {
    struct location *castle;
    struct location *lair;
};

// `struct location` represents a location, which could be a land,
// a tower, or a castle. Each of those places has a different type
// of information that this struct will need to store.
//
// You will need to add fields to struct location.
struct location {
    char name[MAX_NAME_LENGTH];
    int power;
    int uses;
    int hp;
    int effect;
    struct enemy *enemies;
    struct location *next;
};

// `struct enemy` represents an enemy, which will move through the
// realm (to each location). Towers can deal damage to it, and
// it can deal damage to the Castle.
//
// You will need to add fields to struct enemy.
struct enemy {
    int hp;
    int max_hp;
    char name[MAX_NAME_LENGTH];
    struct enemy *next;
};

// Add any other structs you define here.

///////////////////////////////////////////////////////////////////////

// Function prototypes for helper functions
static Location new_location(char *name);
static void print_tower(char *name, int power, int uses, Effect effect);
static void print_land(char *name);
static void print_castle(char *name, int defense);
static void print_enemy(char *name, int cur_hp, int max_hp);


// Add prototypes for any extra functions you create here.
static int damage_enemies(Realm realm);
static int enemies_length(struct enemy *head);
static void remove_dead_enemies(Realm realm);
static Enemy find_healthy_enemy(struct enemy *head);
static int compare_string(char *search_term, char *name, int max_length);
static int remove_enemies_at_location(struct location *head);
static void alphabetial_enemies(struct location *head, int length);
static void sort_all_enemies(struct location *head); 
static void move_ice_enemies(Realm realm, struct location *head);
static void move_portal_enemies(Realm realm, struct location *head);
static int enemy_hp_increase (
           struct location *head, char *search_term, int amount);
static int tower_power_increase (
           struct location *head, char *search_term, int amount);
static int tower_uses_increase (
           struct location *head, char *search_term, int amount);
static Enemy find_last_enemy(struct enemy *head);
static int char_class_success(char *search_term, char named, 
                              int search_pos, int success);
///////////////////////////////////////////////////////////////////////

// You need to implement the following 9 functions.
// In other words, write code to make the function work as described 
// in realm.h.

// Create a new realm, and return a pointer to it.
// You may need to change this function in later stages.
Realm new_realm(void) {
    struct realm *realm = malloc(sizeof(struct realm));
    
    // A realm initially only has a castle and lair and the castle points to
    // the lair as the next location. The castle has a starting health of 100 
    realm->castle = new_location("Castle");
    realm->lair = new_location("Lair");
   
    realm->castle->next = realm->lair;
    realm->castle->hp = STARTING_CASTLE_HP;

    return realm;
}


// Return a new location created with malloc.
static Location new_location(char *name) {

    struct location *created_location = malloc(sizeof(struct location));
    
    // Initialises all the variables in a location
    strcpy(created_location->name, name);
    created_location->enemies = NULL;
    created_location->next = NULL;
    created_location->power = 0;
    created_location->uses = 0;
    created_location->hp = 0;
    created_location->effect = EFFECT_NONE;

    return created_location;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

// Add a new location to the realm, and return the total number of
// locations in the realm.
int add_location(Realm realm, char *name) {
    
    // Stores the total number of locations in the realm
    int location_counter = 0; 
    
    struct location *created_location = new_location(name);
    struct location *head = realm->castle;
    
    // Searches through the linked list of locations to find how many 
    // locations there are and adds a new location just before the lair
    while (head != NULL) {
        location_counter++;
        
        if (head->next == realm->lair) {
            // adds the new location to the linked list in position before the 
            // lair
            head->next = created_location;
            created_location->next = realm->lair;
            
            // The + 2 counts the lair and the created location
            location_counter = location_counter + 2;
            
            return location_counter;
             
        }
        
        head = head->next;
    }
    
    return location_counter; 
}

// Print out the realm.

void print_realm(Realm realm) {
    struct location *head = realm->castle;

    while (head != NULL) {
        // There is a special print function for specifically a castle
        if (strcmp(head->name, "Castle") == 0) {
            print_castle(head->name, head->hp);
        }
        // If a tower's power or uses falls below 1, we treat them like a land
        // Otherwise, all lands have power and uses 0, so this will always
        // be true for a land
        else if (head->power < MIN_POWER || head->uses < MIN_USES) {
            print_land(head->name);
        }
        else {
            print_tower(head->name, head->power, head->uses, head->effect);
        }
        
        struct enemy *enemy_head = head->enemies;
        
        // Prints all the enemies in the location
        while (enemy_head != NULL) {
            print_enemy(enemy_head->name, enemy_head->hp, enemy_head->max_hp);
            enemy_head = enemy_head->next;
        }

        head = head->next;
    }
    // Note: you should use the provided print functions here.
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

// Add an enemy to the realm.
int new_enemy(Realm realm, char *location_name, char *name, int hp) {
    struct location *head = realm->castle;
    
    // Finds the location that matches the location name
    while (head != NULL && strcmp(head->name, location_name) != 0) {
        head = head->next;
    }
    
    // Ensures that there is both a location and a valid hp
    if (head == NULL) {
        return ERROR_NO_LOCATION;
    }
    else if (hp < MIN_HP) {
        return ERROR_INVALID_STAT;
    }
    
    
    struct enemy *constructed_enemy = malloc(sizeof(struct enemy));
    
    // Initialises the values in an enemy
    strcpy(constructed_enemy->name, name);
    constructed_enemy->hp = hp;
    constructed_enemy->max_hp = hp;
    constructed_enemy->next = NULL;
    
    // If there is no enemy at the location, the new enemy created becomes
    // the only enemy there
    if (head->enemies == NULL) {
        head->enemies = constructed_enemy;
    }
    // Otherwise, it finds the last enemy in the linked list and then 
    // adds the enemy on to the end
    else {
        struct enemy *enemy_head = find_last_enemy(head->enemies);
    
        enemy_head->next = constructed_enemy;
    
    }
    
    return SUCCESS;
}

// Add a new tower to the realm.
int new_tower(Realm realm, char *prev_name, char *name, int power, int uses) {
    struct location *head = realm->castle;
    
    // A tower must have at minimum one power and one use
    if (power < MIN_POWER || uses < MIN_USES) {
        return ERROR_INVALID_STAT;
    }
    
    struct location *constructed_tower = new_location(name);
    
    // Initialises the value in a tower
    constructed_tower->power = power;
    constructed_tower->uses = uses;
    
    // Finds the location that matches where we want to place the tower directly
    // after
    while (head != NULL && strcmp(head->name, prev_name) != 0) {
        head = head->next;
    }
    
    if (head == NULL) {
        return ERROR_NO_LOCATION;
    }
    // Inserts that location into the linked list 
    else {
        constructed_tower->next = head->next;
        head->next = constructed_tower;
    }
    
    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////


// Removes the enemies at a location and returns the amount of enemies removed
static int remove_enemies_at_location(struct location *head) {

    int enemies_removed = 0;
    
    struct enemy *enemy_head = head->enemies;
        
    while (enemy_head != NULL) {
        // Stores the pointer for an enemy and moves on to the next enemy 
        // before freeing the memeory associated with the first enemy
        struct enemy *enemy_to_free = enemy_head;
    
        enemy_head = enemy_head->next;
        enemies_removed++;
        
        free(enemy_to_free);
    }
    return enemies_removed;
}

// Destroy the realm, and free any associated memory.
void destroy_realm(Realm realm) {
    struct location *head = realm->castle;
    
    while (head != NULL) {
        // Moves through each location and frees the memory associated with
        // each enemy at the location as well as the location itself.
        struct location *free_location = head;
        remove_enemies_at_location(head);
        
        head = head->next;
        free(free_location);
        
    }
    
    // Finally, frees the memory associated with the realm
    free(realm);

}



// Advance enemies towards the castle. Enemies at the castle are removed and 
// the amount of enemies removed in this manner are returned
int advance_enemies(Realm realm) {
    
    struct location *head = realm->castle;
    struct enemy *enemy_head = head->enemies;
    
    // Removes all enemies at the castle first
    int enemies_removed = remove_enemies_at_location(head);
    
    while (head->next != NULL) {
        // This takes in enemies at the ice tower and then 
        // separates them to two different locations (either advancing them or
        // forcing them to stay at the same location
        if (head->next->effect == EFFECT_ICE && head->next->enemies != NULL) {
            move_ice_enemies(realm, head);
        }
        else {
            enemy_head = head->enemies;
            // The enemies at the ice tower stay at the ice tower and so we
            // must add to the end of that linked list of enemies to combine
            // all the enemies to one location
            if (head->effect == EFFECT_ICE && enemy_head != NULL) {
                enemy_head = find_last_enemy(head->enemies);
                enemy_head->next = head->next->enemies;
            }
            // This is the normal case: Enemies move forward by one step
            // and replace whatever was there
            else {
                head->enemies = head->next->enemies;
            }
        }
        head = head->next; 
    }
    
    
    // After all enemies have advanced a step, there should be no enemies at 
    // the lair pre-portal movement
    head->enemies = NULL;
    
    // After movement, enemies at a castle are teleported back to the lair
    move_portal_enemies(realm, realm->castle);
    
    // Sorts the enemies at all locations alphabetically
    sort_all_enemies(realm->castle);

    return enemies_removed;
    
}


// Apply damage from the enemies at each tower to that tower and return how
// many enemies were damaged in this way.
int apply_damage(Realm realm) {

    // Damage the enemies and kill the ones with hp <= 0
    int enemies_damaged = damage_enemies(realm);
    remove_dead_enemies(realm);
    
    // For each enemy at the castle, deal damage to the castle by the 
    // enemy's hp
    struct enemy *enemy_head = realm->castle->enemies;
    while (enemy_head != NULL) {
        realm->castle->hp = realm->castle->hp - enemy_head->hp;
        enemy_head = enemy_head->next;
    }
    
    return enemies_damaged;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////


// Apply the specified buff to the relevant towers or enemies and return the
// amount of towers/enemies buffed in this manner.
int apply_buff(Realm realm, char *search_term, Buff buff, int amount){
    struct location *head = realm->castle;
    
    if (buff == BUFF_ENEMY_HP) {
        // All enemies with names that match the search term have their hp
        // increase by the amount
        int enemy_counter = enemy_hp_increase(head, search_term, amount);
        remove_dead_enemies(realm);
        
        return enemy_counter;    
    }
    else if (buff == BUFF_TOWER_POWER) {
        // All towers with names that match the search term have their power
        // increase by the amount
        int power_counter = tower_power_increase(head, search_term, amount);
        
        return power_counter;    
    }
    else if (buff == BUFF_TOWER_USES) {
        // All towers with names that match the search term have their uses
        // increase by the amount
        int uses_counter = tower_uses_increase(head, search_term, amount);
        
        return uses_counter;    
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////

// Apply the specified effect to each tower whose name matches the
// specified search term and return the amount of towers changed in this manner.
int apply_effect(Realm realm, char *search_term, Effect effect) {
    struct location *head = realm->castle;
    
    int tower_counter = 0;
    while (head != NULL) {
        // Checks whether a tower matches a search term
        int fail = compare_string(search_term, head->name, strlen(head->name));
        
        // If it does, it grants them the effect            
        if (!fail) {
            head->effect = effect;
            tower_counter++;
        }
        head = head->next;
    }
    
   
    return tower_counter;
}

////////////////////////////////////////////////////////////////////////

// Add definitions for your own functions below.
// Make them static to limit their scope to this file.


// Runs through all the locations and deals damage to all enemies being
// blasted by a tower. Returns the amount of enemies damaged in this manner
static int damage_enemies(Realm realm) {
    struct location *head = realm->castle;
    
    int total_enemies = 0;
    
    while (head != NULL) {
        int enemy_counter = 0;
        // Checks that the location is a tower
        if (head->power >= MIN_POWER && head->uses >= MIN_USES) {
            struct enemy *enemy_head = head->enemies;
            while (enemy_head != NULL) {
                // damages all enemies hit by the tower
                enemy_head->hp = enemy_head->hp - head->power;
                enemy_head = enemy_head->next;
                enemy_counter++;
            }
            
            // If there is an enemy hit, then the amount of uses the tower has
            // decreases and if it runs out of uses, it powers off
            if (enemy_counter > 0) {
                head->uses = head->uses - 1;
            }
            if (head->uses < MIN_USES) {
                head->power = 0;
            }
        }
        head = head->next;
        total_enemies = total_enemies + enemy_counter;
    }
    return total_enemies;
    
}

// Finds the first alive enemy in a linked list of enemies. Any dead enemies
// are freed
static Enemy find_healthy_enemy(struct enemy *head){
    while (head != NULL) {
        if (head->hp >= MIN_HP) {
            break;
        }
        struct enemy *enemy_to_free = head;
    
        head = head->next;
        free(enemy_to_free);
    }   
    
    return head;
}

// Finds the length of a linked list of enemies
static int enemies_length(struct enemy *head) {
    int counter = 0;
    while (head != NULL) {
        head = head->next;
        counter++;
    }
    return counter;
}

static int compare_string(char *search_term, char *name, int max_length) {
    int i = 0;
   
    int fail = 0;
    int success = 0;
    
    // Whenever 'choose_between' gets set to 1, it means that we need to compare 
    // all characters inside the brackets [] with the matching character in 
    // the name
    int choose_between = 0;
    
    // The counter is to make sure that whenever a special character is found,
    // that it doesn't impact what is being compared.
    int counter = 0;
    
    // Iterates through all the characters in the search term. The idea is that
    // if there is a single failure, then the name doesn't match the search term
    // so this single failure is all it looks for.
    while (i < strlen(search_term)) {
        // The search will fail if the search term is greater in length than 
        // the name
        if (i - counter >= max_length) {
            fail = 1;
            return fail;
        }
        
        char searched = search_term[i];
        char named = name[i - counter];
        
        // The beginning of comparing all chars inside the bracket to a 
        // character
        if (searched == '[') {
            choose_between = 1;
            counter++;
        }
        // The end of comparing all chars inside the bracket to a 
        // character
        else if (searched == ']' && choose_between == 1) {
            choose_between = 0;
            // If not a single char inside the brackets matched the character 
            // that corresponded, then the search fails
            if (!success) {
                fail = 1;
                return fail;
            }
            success = 0;
            
        }
        // Attempts to find a single character inside the brackets that 
        // matches the corresponding character
        else if (choose_between) {
            if (searched == named) {
                success = 1;
            }
            // Searches in between the two characters/numbers to the side
            // of this to compare them with the corresponding character
            else if (searched == '-') {
                success = char_class_success(search_term, named, i, success);
               
            }
            counter++;
        }
        else if (searched != named) {
            fail = 1;
        }

        i++;
    }
    return fail;
}

// Say, we need to search A-Z
// This checks that first, it is ascending in ASCII value
// That is, Z-A wouldn't work
// Then it checks how many terms we need to search in between 
// the character class 
// Then it iterates through the characters and checks until it reaches 
// just before the final character (in the hypothetical case, 'Z')
static int char_class_success(char *search_term, char named, 
                              int search_pos, int success) {
    
    int terms_to_search = search_term[search_pos + 1] - search_term[search_pos - 1] - 1;
    int i = 0;
    if (terms_to_search > 0) {
        while (i <= terms_to_search) {
            // Goes through all the ASCII chars in acending order to compare
            char check = search_term[search_pos - 1] + i;
            if (check == named) {
                success = 1;
            }
            i ++;
        }
    }
    return success;
}

static void remove_dead_enemies(Realm realm) {
    struct location *head = realm->castle;
    
    while (head != NULL) {
        struct enemy *enemy_head = head->enemies;
        
        // First find an enemy that isn't dead
        enemy_head = find_healthy_enemy(enemy_head);
        
        //Set the head enemy to the alive enemy
        head->enemies = enemy_head;
        
        // Makes sure there is such a thing as an alive enemy. Then iterates
        // finding an enemy that isn't dead and then adding it on to the linked
        // list of enemies
        if (enemy_head != NULL) {
            struct enemy *find_next_enemy = enemy_head->next;
        
            while (find_next_enemy != NULL) {
                find_next_enemy = find_healthy_enemy(find_next_enemy);
            
                enemy_head->next = find_next_enemy;
                enemy_head = find_next_enemy;
            
                find_next_enemy = find_next_enemy->next;
            }
        
        }

        head = head->next;
    }
}

// This applies a bubble sort on the enemies until they are sorted in 
// alphabetical order
static void alphabetial_enemies(struct location *head, int length) {

    int i = 0;
    
    // Each iteration of this bubble-sort swapping will put at least one
    // enemy in the right position each time so the maximum amount of 
    // iterations is the amount of enemies in the linked list
    while (i < length) {
        struct enemy *enemy_head = head->enemies;
        while (enemy_head->next != NULL) {
            // If two enemies are not alphabetically sorted in the linked list
            // already, swap the two enemies around until you reach the end of
            // the linked list
            if (strcmp(enemy_head->name, enemy_head->next->name) > 0) {
                int store_hp = enemy_head->hp;
                int store_max_hp = enemy_head->max_hp;
                char store_name[MAX_NAME_LENGTH];
                    
                strcpy(store_name, enemy_head->name);
                    
                enemy_head->hp = enemy_head->next->hp;
                enemy_head->max_hp = enemy_head->next->max_hp;
                strcpy(enemy_head->name, enemy_head->next->name);
                      
                enemy_head->next->hp = store_hp;
                enemy_head->next->max_hp = store_max_hp;
                strcpy(enemy_head->next->name, store_name);      
            }
            enemy_head = enemy_head->next;
        }
        i++;
    }
}

// If there are enemies standing on a portal, this moves them all to the lair
static void move_portal_enemies(Realm realm, struct location *head) {
    while (head->next != NULL) {
        if (head->effect == EFFECT_PORTAL && head->enemies != NULL) {
            if (realm->lair->enemies == NULL) {
                realm->lair->enemies = head->enemies;
            }
            else {
                struct enemy *enemy_head = head->enemies;
                struct enemy *lair_head = find_last_enemy(realm->lair->enemies);

                lair_head->next = enemy_head;
                
            }
            head->enemies = NULL;
            head->effect = EFFECT_NONE;  
        }
        head = head->next; 
    }
}

// All enemies at all locations are sorted alphabetically
static void sort_all_enemies(struct location *head) {
    while (head != NULL) {
        struct enemy *enemy_head = head->enemies;
        int length = enemies_length(enemy_head);
       
        if (enemy_head != NULL) {
            alphabetial_enemies(head, length);
        }
        head = head->next;
    }
}

// All enemies with names that match the search term have their hp
// increase by the amount
static int enemy_hp_increase (struct location *head, char *search_term, int amount) {
    int enemy_counter = 0;
    while (head != NULL) {
        struct enemy *enemy_head = head->enemies;
        while (enemy_head != NULL) {
            int fail = compare_string(search_term, enemy_head->name, 
                                      strlen(enemy_head->name));
            if (!fail) {
                enemy_head->hp = enemy_head->hp + amount;
                enemy_counter++;
            }
            enemy_head = enemy_head->next;
        }
        head = head->next;
    }
    
    return enemy_counter;
}

// All towers with names that match the search term have their power
// increase by the amount
static int tower_power_increase (struct location *head, char *search_term, int amount) {
    int power_counter = 0;
    while (head != NULL) {
        if (head->power >= MIN_POWER && head->uses >= MIN_USES) {
            int fail = compare_string(search_term, head->name, strlen(head->name));
            if (!fail) {
                head->power = head->power + amount;
                power_counter++;
            }
        }    
        head = head->next;
    }
    return power_counter;    
}

// All towers with names that match the search term have their uses
// increase by the amount
static int tower_uses_increase (struct location *head, char *search_term, int amount) {
    int uses_counter = 0;
    while (head != NULL) {
        if (head->power >= MIN_POWER && head->uses >= MIN_USES) {
            int fail = compare_string(search_term, head->name, strlen(head->name));
            if (!fail) {
                head->uses = head->uses + amount;
                uses_counter++;
            }
        }
        head = head->next;
    } 
    return uses_counter;    
}

// Finds the last enemy in a linked list of enemies
static Enemy find_last_enemy(struct enemy *head) {
    if (head != NULL) {
        while (head->next != NULL) {
            head = head->next;
        }
    }
    return head;
}

// The movement logic for advancing enemies located at an ice tower
static void move_ice_enemies(Realm realm, struct location *head) {
    struct enemy *next_head = head->next->enemies;
    
    // Changes the enemies stored at the location where an enemy would
    // normally advance to and the ice tower location to NULL.     
    head->enemies = NULL;
    head->next->enemies = NULL;
            
    while (next_head != NULL) {
        // We separate the enemies at the ice tower here by creating 
        // new enemies with the same attributes as was on the ice tower 
        // onto their respective (next location/still on ice tower)
        if (next_head->hp < head->next->power) {
            new_enemy(realm, head->next->name, 
            next_head->name, next_head->hp);
        }
        else {
            new_enemy(realm, head->name, 
            next_head->name, next_head->hp);
        }
        struct enemy *store = next_head;
        next_head = next_head->next;
        // We free the enemies that were copied in this manner
        free(store);
    }
}
////////////////////////////////////////////////////////////////////////
//                    Provided print functions                        //
//         NOTE: YOU SHOULD NOT MODIFY THE FOLLOWING FUNCTIONS        //
////////////////////////////////////////////////////////////////////////

void print_tower(char *name, int power, int uses, int effect) {
    printf(" ^ %32s [pow: %3d | uses: %3d]", name, power, uses);
    if (effect == EFFECT_NONE) printf(" {%c}", EFFECT_NONE_CHAR);
    if (effect == EFFECT_PORTAL) printf(" {%c}", EFFECT_PORTAL_CHAR);
    if (effect == EFFECT_ICE) printf(" {%c}", EFFECT_ICE_CHAR);
    printf("\n");
}

void print_land(char *name) {
    printf(" ^ %32s [____________________]\n", name);
}

void print_castle(char *name, int defense) {
    printf(" ^ %32s [Castle Defenses: %3d]\n", name, defense);
}

void print_enemy(char *name, int cur_hp, int max_hp) {
    printf(" ^ %40s [hp: %d/%d]\n", name, cur_hp, max_hp);
}

////////////////////////////////////////////////////////////////////////
//               End of provided print functions                      //
////////////////////////////////////////////////////////////////////////



