/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_hashtable_names.h"

/* this is over-documented because it was almost a public API. Leaving the
   full docs here in case it _does_ become public some day. */

/* WIKI CATEGORY: HashTable */

/**
 * # CategoryHashTable
 *
 * SDL offers a hash table implementation, as a convenience for C code that
 * needs efficient organization and access of arbitrary data.
 *
 * Hash tables are a popular data structure, designed to make it quick to
 * store and look up arbitrary data. Data is stored with an associated "key."
 * While one would look up an element of an array with an index, a hash table
 * uses a unique key to find an element later.
 *
 * A key can be anything, as long as its unique and in a format that the table
 * understands. For example, it's popular to use strings as keys: the key
 * might be a username, and it is used to lookup account information for that
 * user, etc.
 *
 * Hash tables are named because they "hash" their keys down into simple
 * integers that can be used to efficiently organize and access the associated
 * data.
 *
 * As this is a C API, there is one generic interface that is intended to work
 * with different data types. This can be a little awkward to set up, but is
 * easy to use after that.
 *
 * Hashtables are generated by a call to SDL_CreateHashTable(). This function
 * requires several callbacks to be provided (for hashing keys, comparing
 * entries, and cleaning up entries when removed). These are necessary to
 * allow the hash to manage any arbitrary data type.
 *
 * Once a hash table is created, the common tasks are inserting data into the
 * table, (SDL_InsertIntoHashTable), looking up previously inserted data
 * (SDL_FindInHashTable), and removing data (SDL_RemoveFromHashTable and
 * SDL_ClearHashTable). Less common but still useful is the ability to
 * iterate through all the items in the table (SDL_IterateHashTable).
 *
 * The underlying hash table implementation is always subject to change, but
 * at the time of writing, it uses open addressing and Robin Hood hashing.
 * The technical details are explained [here](https://github.com/libsdl-org/SDL/pull/10897).
 *
 * Hashtables keep an SDL_RWLock internally, so multiple threads can perform
 * hash lookups in parallel, while changes to the table will safely serialize
 * access between threads.
 *
 * SDL provides a layer on top of this hash table implementation that might be
 * more pleasant to use. SDL_PropertiesID maps a string to arbitrary data of
 * various types in the same table, which could be both easier to use and more
 * flexible. Refer to [CategoryProperties](CategoryProperties) for details.
 */

#ifndef SDL_hashtable_h_
#define SDL_hashtable_h_

#include <SDL3/SDL_stdinc.h>

#include <SDL3/SDL_begin_code.h>
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * The opaque type that represents a hash table.
 *
 * This is hidden behind an opaque pointer because not only does the table
 * need to store arbitrary data types, but the hash table implementation may
 * change in the future.
 *
 * \since This struct is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
typedef struct SDL_HashTable SDL_HashTable;

/**
 * A function pointer representing a hash table hashing callback.
 *
 * This is called by SDL_HashTable when it needs to look up a key in
 * its dataset. It generates a hash value from that key, and then uses that
 * value as a basis for an index into an internal array.
 *
 * There are no rules on what hashing algorithm is used, so long as it
 * can produce a reliable 32-bit value from `key`, and ideally distributes
 * those values well across the 32-bit value space. The quality of a
 * hashing algorithm is directly related to how well a hash table performs.
 *
 * Hashing can be a complicated subject, and often times what works best
 * for one dataset will be suboptimal for another. There is a good discussion
 * of the field [on Wikipedia](https://en.wikipedia.org/wiki/Hash_function).
 *
 * Also: do you _need_ to write a hashing function? SDL provides generic
 * functions for strings (SDL_HashString), generic integer IDs (SDL_HashID),
 * and generic pointers (SDL_HashPointer). Often you should use one of these
 * before writing your own.
 *
 * \param userdata what was passed as `userdata` to SDL_CreateHashTable().
 * \param key the key to be hashed.
 * \returns a 32-bit value that represents a hash of `key`.
 *
 * \threadsafety This function must be thread safe if the hash table is used
 *               from multiple threads at the same time.
 *
 * \since This datatype is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 * \sa SDL_HashString
 * \sa SDL_HashID
 * \sa SDL_HashPointer
 */
typedef Uint32 (SDLCALL *SDL_HashCallback)(void *userdata, const void *key);


/**
 * A function pointer representing a hash table matching callback.
 *
 * This is called by SDL_HashTable when it needs to look up a key in its
 * dataset. After hashing the key, it looks for items stored in relation to
 * that hash value. Since there can be more than one item found through the
 * same hash value, this function verifies a specific value is actually
 * correct before choosing it.
 *
 * So this function needs to compare the keys at `a` and `b` and decide if
 * they are actually the same.
 *
 * For example, if the keys are C strings, this function might just be:
 *
 * ```c
 * return (SDL_strcmp((const char *) a, const char *b) == 0);`
 * ```
 *
 * Also: do you _need_ to write a matching function? SDL provides generic
 * functions for strings (SDL_KeyMatchString), generic integer IDs
 * (SDL_KeyMatchID), and generic pointers (SDL_KeyMatchPointer). Often you
 * should use one of these before writing your own.
 *
 * \param userdata what was passed as `userdata` to SDL_CreateHashTable().
 * \param a the first key to be compared.
 * \param b the second key to be compared.
 * \returns true if two keys are identical, false otherwise.
 *
 * \threadsafety This function must be thread safe if the hash table is used
 *               from multiple threads at the same time.
 *
 * \since This datatype is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
typedef bool (SDLCALL *SDL_HashKeyMatchCallback)(void *userdata, const void *a, const void *b);


/**
 * A function pointer representing a hash table cleanup callback.
 *
 * This is called by SDL_HashTable when removing items from the hash, or
 * destroying the hash table. It is used to optionally deallocate the
 * key/value pairs.
 *
 * This is not required to do anything, if all the data in the table is
 * static or POD data, but it can also do more than a simple free: for
 * example, if the hash table is storing open files, it can close them here.
 * It can also free only the key or only the value; it depends on what the
 * hash table contains.
 *
 * \param userdata what was passed as `userdata` to SDL_CreateHashTable().
 * \param key the key to deallocate.
 * \param value the value to deallocate.
 *
 * \threadsafety This function must be thread safe if the hash table is used
 *               from multiple threads at the same time.
 *
 * \since This datatype is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
typedef void (SDLCALL *SDL_HashDestroyCallback)(void *userdata, const void *key, const void *value);


/**
 * A function pointer representing a hash table iterator callback.
 *
 * This function is called once for each key/value pair to be considered
 * when iterating a hash table.
 *
 * Iteration continues as long as there are more items to examine and this
 * callback continues to return true.
 *
 * Do not attempt to modify the hash table during this callback, as it will
 * cause incorrect behavior and possibly crashes.
 *
 * \param userdata what was passed as `userdata` to an iterator function.
 * \param table the hash table being iterated.
 * \param key the current key being iterated.
 * \param value the current value being iterated.
 * \returns true to keep iterating, false to stop iteration.
 *
 * \threadsafety A read lock is held during iteration, so other threads can
 *               still access the the hash table, but threads attempting to
 *               make changes will be blocked until iteration completes. If
 *               this is a concern, do as little in the callback as possible
 *               and finish iteration quickly.
 *
 * \since This datatype is available since SDL 3.4.0.
 *
 * \sa SDL_IterateHashTable
 */
typedef bool (SDLCALL *SDL_HashTableIterateCallback)(void *userdata, const SDL_HashTable *table, const void *key, const void *value);


/**
 * Create a new hash table.
 *
 * To deal with different datatypes and needs of the caller, hash tables
 * require several callbacks that deal with some specifics: how to hash a key,
 * how to compare a key for equality, and how to clean up keys and values.
 * SDL provides a few generic functions that can be used for these callbacks:
 *
 * - SDL_HashString and SDL_KeyMatchString for C strings.
 * - SDL_HashPointer and SDL_KeyMatchPointer for generic pointers.
 * - SDL_HashID and SDL_KeyMatchID for generic (possibly small) integers.
 *
 * Oftentimes, these are all you need for any hash table, but depending on
 * your dataset, custom implementations might make more sense.
 *
 * You can specify an estimate of the number of items expected to be stored
 * in the table, which can help make the table run more efficiently. The table
 * will preallocate resources to accomodate this number of items, which is
 * most useful if you intend to fill the table with a lot of data right after
 * creating it. Otherwise, it might make more sense to specify the _minimum_
 * you expect the table to hold and let it grow as necessary from there. This
 * number is only a hint, and the table will be able to handle any amount of
 * data--as long as the system doesn't run out of resources--so a perfect
 * answer is not required. A value of 0 signifies no guess at all, and the
 * table will start small and reallocate as necessary; often this is the
 * correct thing to do.
 *
 * !!! FIXME: add note about `threadsafe` here. And update `threadsafety` tags.
 * !!! FIXME: note that `threadsafe` tables can't be recursively locked, so
 * !!! FIXME:  you can't use `destroy` callbacks that might end up relocking.
 *
 * Note that SDL provides a higher-level option built on its hash tables:
 * SDL_PropertiesID lets you map strings to various datatypes, and this
 * might be easier to use. It only allows strings for keys, however. Those are
 * created with SDL_CreateProperties().
 *
 * The returned hash table should be destroyed with SDL_DestroyHashTable()
 * when no longer needed.
 *
 * \param estimated_capacity the approximate maximum number of items to be held
 *                           in the hash table, or 0 for no estimate.
 * \param threadsafe true to create an internal rwlock for this table.
 * \param hash the function to use to hash keys.
 * \param keymatch the function to use to compare keys.
 * \param destroy the function to use to clean up keys and values, may be NULL.
 * \param userdata a pointer that is passed to the callbacks.
 * \returns a newly-created hash table, or NULL if there was an error; call
 *          SDL_GetError() for more information.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_DestroyHashTable
 */
extern SDL_HashTable * SDL_CreateHashTable(int estimated_capacity,
                                           bool threadsafe,
                                           SDL_HashCallback hash,
                                           SDL_HashKeyMatchCallback keymatch,
                                           SDL_HashDestroyCallback destroy,
                                           void *userdata);


/**
 * Destroy a hash table.
 *
 * This will call the hash table's SDL_HashDestroyCallback for each item in
 * the table, removing all inserted items, before deallocating the table
 * itself.
 *
 * The table becomes invalid once this function is called, and no other thread
 * should be accessing this table once this function has started.
 *
 * \param table the hash table to destroy.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 */
extern void SDL_DestroyHashTable(SDL_HashTable *table);

/**
 * Add an item to a hash table.
 *
 * All keys in the table must be unique. If attempting to insert a key that
 * already exists in the hash table, what will be done depends on the
 * `replace` value:
 *
 * - If `replace` is false, this function will return false without modifying
 *   the table.
 * - If `replace` is true, SDL will remove the previous item first, so the new
 *   value is the only one associated with that key. This will call the hash
 *   table's SDL_HashDestroyCallback for the previous item.
 *
 * \param table the hash table to insert into.
 * \param key the key of the new item to insert.
 * \param value the value of the new item to insert.
 * \param replace true if a duplicate key should replace the previous value.
 * \returns true if the new item was inserted, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 */
extern bool SDL_InsertIntoHashTable(SDL_HashTable *table, const void *key, const void *value, bool replace);

/**
 * Look up an item in a hash table.
 *
 * On return, the value associated with `key` is stored to `*value`.
 * If the key does not exist in the table, `*value` will be set to NULL.
 *
 * It is legal for `value` to be NULL, to not retrieve the key's value. In
 * this case, the return value is still useful for reporting if the key exists
 * in the table at all.
 *
 * \param table the hash table to search.
 * \param key the key to search for in the table.
 * \param value the found value will be stored here. Can be NULL.
 * \returns true if key exists in the table, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_InsertIntoHashTable
 */
extern bool SDL_FindInHashTable(const SDL_HashTable *table, const void *key, const void **value);

/**
 * Remove an item from a hash table.
 *
 * If there is an item that matches `key`, it is removed from the table. This
 * will call the hash table's SDL_HashDestroyCallback for the item to be
 * removed.
 *
 * \param table the hash table to remove from.
 * \param key the key of the item to remove from the table.
 * \returns true if a key was removed, false if the key was not found.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 */
extern bool SDL_RemoveFromHashTable(SDL_HashTable *table, const void *key);

/**
 * Remove all items in a hash table.
 *
 * This will call the hash table's SDL_HashDestroyCallback for each item in
 * the table, removing all inserted items.
 *
 * When this function returns, the hash table will be empty.
 *
 * \param table the hash table to clear.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 */
extern void SDL_ClearHashTable(SDL_HashTable *table);

/**
 * Check if any items are currently stored in a hash table.
 *
 * If there are no items stored (the table is completely empty), this will
 * return true.
 *
 * \param table the hash table to check.
 * \returns true if the table is completely empty, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_ClearHashTable
 */
extern bool SDL_HashTableEmpty(SDL_HashTable *table);

/**
 * Iterate all key/value pairs in a hash table.
 *
 * This function will call `callback` once for each key/value pair in the
 * table, until either all pairs have been presented to the callback, or the
 * callback has returned false to signal it is done.
 *
 * There is no guarantee what order results will be returned in.
 *
 * \param table the hash table to iterate.
 * \param callback the function pointer to call for each value.
 * \param userdata a pointer that is passed to `callback`.
 * \returns true if iteration happened, false if not (bogus parameter, etc).
 *
 * \since This function is available since SDL 3.4.0.
 */
extern bool SDL_IterateHashTable(const SDL_HashTable *table, SDL_HashTableIterateCallback callback, void *userdata);


/* Helper functions for SDL_CreateHashTable callbacks... */

/**
 * Generate a hash from a generic pointer.
 *
 * The key is intended to be a unique pointer to any datatype.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * Note that the implementation may change in the future; do not expect
 * the results to be stable vs future SDL releases. Use this in a hash table
 * in the current process and don't store them to disk for the future.
 *
 * \param unused this parameter is ignored.
 * \param key the key to hash as a generic pointer.
 * \returns a 32-bit hash of the key.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern Uint32 SDL_HashPointer(void *unused, const void *key);

/**
 * Compare two generic pointers as hash table keys.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * \param unused this parameter is ignored.
 * \param a the first generic pointer to compare.
 * \param b the second generic pointer to compare.
 * \returns true if the pointers are the same, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern bool SDL_KeyMatchPointer(void *unused, const void *a, const void *b);

/**
 * Generate a hash from a C string.
 *
 * The key is intended to be a NULL-terminated string, in UTF-8 format.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * Note that the implementation may change in the future; do not expect
 * the results to be stable vs future SDL releases. Use this in a hash table
 * in the current process and don't store them to disk for the future.
 *
 * \param unused this parameter is ignored.
 * \param key the key to hash as a generic pointer.
 * \returns a 32-bit hash of the key.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern Uint32 SDL_HashString(void *unused, const void *key);

/**
 * Compare two C strings as hash table keys.
 *
 * Strings will be compared in a case-sensitive manner. More specifically,
 * they'll be compared as NULL-terminated arrays of bytes.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * \param unused this parameter is ignored.
 * \param a the first string to compare.
 * \param b the second string to compare.
 * \returns true if the strings are the same, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern bool SDL_KeyMatchString(void *unused, const void *a, const void *b);

/**
 * Generate a hash from an integer ID.
 *
 * The key is intended to a unique integer, possibly within a small range.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * Note that the implementation may change in the future; do not expect
 * the results to be stable vs future SDL releases. Use this in a hash table
 * in the current process and don't store them to disk for the future.
 *
 * \param unused this parameter is ignored.
 * \param key the key to hash as a generic pointer.
 * \returns a 32-bit hash of the key.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern Uint32 SDL_HashID(void *unused, const void *key);

/**
 * Compare two integer IDs as hash table keys.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of keys to be used with the hash table.
 *
 * \param unused this parameter is ignored.
 * \param a the first ID to compare.
 * \param b the second ID to compare.
 * \returns true if the IDs are the same, false otherwise.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern bool SDL_KeyMatchID(void *unused, const void *a, const void *b);

/**
 * Free both the key and value pointers of a hash table item.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of data to be used with the hash table.
 *
 * This literally calls `SDL_free(key);` and `SDL_free(value);`.
 *
 * \param unused this parameter is ignored.
 * \param key the key to be destroyed.
 * \param value the value to be destroyed.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern void SDL_DestroyHashKeyAndValue(void *unused, const void *key, const void *value);

/**
 * Free just the value pointer of a hash table item.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of data to be used with the hash table.
 *
 * This literally calls `SDL_free(key);` and leaves `value` alone.
 *
 * \param unused this parameter is ignored.
 * \param key the key to be destroyed.
 * \param value the value to be destroyed.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern void SDL_DestroyHashKey(void *unused, const void *key, const void *value);

/**
 * Free just the value pointer of a hash table item.
 *
 * This is intended to be used as one of the callbacks to SDL_CreateHashTable,
 * if this is useful to the type of data to be used with the hash table.
 *
 * This literally calls `SDL_free(value);` and leaves `key` alone.
 *
 * \param unused this parameter is ignored.
 * \param key the key to be destroyed.
 * \param value the value to be destroyed.
 *
 * \threadsafety It is safe to call this function from any thread.
 *
 * \since This function is available since SDL 3.4.0.
 *
 * \sa SDL_CreateHashTable
 */
extern void SDL_DestroyHashValue(void *unused, const void *key, const void *value);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include <SDL3/SDL_close_code.h>

#endif /* SDL_hashtable_h_ */
