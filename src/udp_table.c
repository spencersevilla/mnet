#include "udp_table.h"

struct udp_table_entry {
    int port;
    struct udp_table_entry *next;
    struct sock *sk;
};

static struct udp_table_entry *head = NULL;

static struct udp_table_entry * udp_table_get_entry_by_port(int port);
static struct udp_table_entry * udp_table_get_entry_by_sk(struct sock *sk);

/* lookup function */
struct sock * udp_table_lookup(int port)
{
    struct udp_table_entry *answer;
    answer = udp_table_get_entry_by_port(port);

    if (answer) {
        return answer->sk;
    }
    return NULL;
}

/* starts a new linked-list 
 * wipes out the old list
 */
static int udp_mhost_table_init(struct udp_table_entry *entry)
{
    head = entry;
    entry->next = NULL;
    return 0;
}

int udp_table_insert(struct sock *sk, int port)
{
    struct udp_table_entry *ptr = head;
    struct udp_table_entry *nxt = NULL;
    struct udp_table_entry *entry;
    
    /* construct and prepare the entry */
    entry = kmalloc(sizeof(struct udp_table_entry), GFP_KERNEL);
    if (!entry) {
        printk(KERN_INFO "udp_table_insert error: kmalloc failed!\n");
        return -1;
    }
    entry->port = port;
    entry->sk = sk;
    entry->next = NULL;
    
    if (head == NULL) {
        /* just in case list was uninitialized */
        return udp_mhost_table_init(entry);
    }
    
    /* case where we have a new list head */
    if (entry->port < head->port) {
        entry->next = head;
        head = entry;
        return 0;
    }
    
    do {
        nxt = ptr->next;
        
        /* insert entry between ptr and nxt 
         * this code also works for when nxt is NULL (EOL)
         */
        if (!nxt || entry->port < nxt->port) {
            if (entry->port == ptr->port) {
                /* ERROR: port already registered! How? */
                printk(KERN_INFO "udp_table_insert error: \
                       port already assigned!\n");
                return -1;
            }
            ptr->next = entry;
            entry->next = nxt;
            return 0;
        }
        
        ptr = nxt;
    } while (1);
    
    /* should never get here? */
    return -1;
}

int udp_table_remove(struct sock *sk)
{
    struct udp_table_entry *past = NULL;
    struct udp_table_entry *entry;
    entry = head;
    
    while (entry) {
        if (sk == entry->sk) {
            break;
        }
        
        past = entry;
        entry = entry->next;
    }
    
    /* NOW entry should point to the correct entry, if it exists
     * and past should point to the one before it (NULL if none)
     * so we can use these two to reconstruct the original linked-list!
     */
    if (entry == NULL) {
        printk(KERN_INFO "udp_table_remove: sk not found!\n");
        return 0;
    }
    
    /* should only happen if entry == head */
    if (past == NULL) {
        if (entry != head) {
            printk(KERN_INFO "udp_table_remove error: confused, \
                   bailing out!!\n");
            return -1;
        }
        
        head = entry->next;
        kfree(entry);
        return 0;
    }
    
    /* works even when entry->next == NULL */
    past->next = entry->next;
    kfree(entry);
    return 0;

}

static struct udp_table_entry * udp_table_get_entry_by_port(int port)
{
    struct udp_table_entry *answer = head;
    while (answer) {
        if (port == answer->port) {
            break;
        }
        
        /* went past it, so we don't have it */
        if (port < answer->port) {
            answer = NULL;
            break;
        }
        
        answer = answer->next;
    }
    
    return answer;
};

static struct udp_table_entry * udp_table_get_entry_by_sk(struct sock *sk)
{
    struct udp_table_entry *answer = head;
    while (answer) {
        if (sk == answer->sk) {
            break;
        }
        
        answer = answer->next;
    }
    
    return answer;
};