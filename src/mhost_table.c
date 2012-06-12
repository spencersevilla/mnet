#include "mhost_table.h"
#include "mhost_inet.h"
#include "mhost_testproto.h"
#include "mhost_otherproto.h"

static int mhost_table_af_specified(struct sockaddr *sa, struct sock *sk);

static struct l3_addr * translate_af_mhost(struct sockaddr_mhost *ma);
static struct l3_binding * binding_from_id(short id);
static int mhost_translate_init(struct l3_binding *binding);
static int mhost_translate_insert(struct l3_binding *binding);
static int binding_insert_addr(struct l3_binding *binding, struct l3_addr *entry);

static struct mhost_proto *head;

/* internal structs for table lookups */
struct l3_addr {
    struct l3_addr *next;   /* linked-list */
    struct mhost_proto *mp; /* has short family as first member! */
    struct sockaddr addr;  /* l3-specific valid address */
};

struct l3_binding {
    short id;                   /* needed until we do getaddrinfo */
    struct l3_binding *next;    /* linked-list */
    struct l3_addr *l3_head;    /* head of l3 address list */
};

static struct l3_binding *table_head = NULL;

/* entry function */
int mhost_table_lookup(struct sockaddr *sa, struct sock *sk)
{
    struct mhost_sock *ms = mhost_sk(sk);
    struct sockaddr_mhost *ma;
    struct l3_addr *addr;
    
    printk(KERN_INFO "called mhost_table_lookup\n");
    
    /* we have no way of ID'ing what's in this sockaddr! */
    if (sa->sa_family == AF_UNSPEC) {
        printk(KERN_INFO "error: cannot read AF_UNSPEC\n");
        return -EAFNOSUPPORT;
    }
    
    /* check if an AF (such as AF_INET) is specified explicitly */
    if (sa->sa_family != AF_MHOST) {
        return mhost_table_af_specified(sa, sk);
    }
    
    /* here we're given an AF_MHOST addressing structure
     * so we must translate the addressing information
     * in addition to finding the right handler!
     */
    
    /* step one: perform lookup on (and/or validate) AF_MHOST pointer.
     * this must be fixed, but in the meanwhile we're using the AF_MHOST
     * quantity as a unique identifier of a node.
     */
    ma = (struct sockaddr_mhost *) sa;
    addr = translate_af_mhost(ma);
    
    
    /* step two: perform any/all sanitation/validation checks and copy
     * all necessary quantities over before returning.
     */
    if (!addr) {
        printk(KERN_INFO "error: mhost node name not found!\n");
        return -EAFNOSUPPORT;
    }

    /* go through the loop: if one l3_addr doesn't have sufficient information,
     * then see if we can find a suitable alternative. If none exist, return bad
     */
    while (addr->mp == NULL) {
        addr = addr->next;
        if (!addr) {
            printk(KERN_INFO "error: no acceptable l3_addr struct!\n");
            return -EAFNOSUPPORT;
        }
    }
    
    /* so: if we get here, we've got all the info we need for translation! */
    ms->proto = addr->mp;
    memcpy(sa, &addr->addr, sizeof(struct sockaddr));
    
    return 0;
};

/*  here, we know that an address family has been explicitly named
 *  all to be done is to find the appropriate set of functions,
 *  set it in the proto field and return. no guarantees here are made
 *  about the validity or correctness of the address given.
 *  NOTE: all protocols must support this feature, as even AF_MHOST
 *  wrappers will just return the same protocol suite called this way.
 */
static int mhost_table_af_specified(struct sockaddr *sa, struct sock *sk)
{
    struct mhost_sock *ms = mhost_sk(sk);
    struct mhost_proto *mp;
    
    mp = mhost_proto_for_family(sa->sa_family);
    if (mp) {
        ms->proto = mp;
        return 0;
    }
    
    return -EAFNOSUPPORT;
};

struct mhost_proto * mhost_proto_for_family(short family)
{
    /* hard-coded for now, but we should really make this "head" */
    struct mhost_proto *answer = head;
    while (answer) {
        if (family == answer->family) {
            break;
        }
        
        /* went past it, so we don't have it */
        if (family < answer->family) {
            answer = NULL;
            break;
        }
        
        /* no more protocols registered */
        if (answer->next == NULL) {
            answer = NULL;
            break;
        }
        
        answer = answer->next;
    }
    
    return answer;
};

/* starts a new linked-list 
 * wipes out the old list
 * DO NOT CALL THIS MORE THAN ONCE!
 */
static int mhost_table_init(struct mhost_proto *proto)
{
    head = proto;
    proto->next = NULL;
    return 0;
}

/* all mhost_proto handlers should call this function
 * to register and put theirselves in the proto linked-list
 */
int mhost_table_register(struct mhost_proto *proto)
{
    struct mhost_proto *ptr = head;
    struct mhost_proto *nxt = NULL;

    if (head == NULL) {
        /* just in case list was uninitialized */
        return mhost_table_init(proto);
    }
    
    /* case where we have a new list head */
    if (proto->family < head->family) {
        proto->next = head;
        head = proto;
        return 0;
    }
    
    do {
        nxt = ptr->next;
        
        /* insert proto between ptr and nxt 
         * this code also works for when nxt is NULL (EOL)
         */
        if (!nxt || proto->family < nxt->family) {
            if (proto->family == ptr->family) {
                /* ERROR: protocol already registered! */
                return -1;
            }
            ptr->next = proto;
            proto->next = nxt;
            return 0;
        }
        
        ptr = nxt;
    } while (1);
    
    /* should never get here? */
    return -1;
}

/* here the following functions all branch from translate_af_mhost
 * which is the major entry-point for returning a coherent l3_addr
 * structure! this is the meat of what AF_MHOST is meant to do.
 */
static struct l3_addr * translate_af_mhost(struct sockaddr_mhost *ma)
{
    /* ALL this function has to do is find the appropriate l3_binding
     * and then return the l3 list head! Integrity is checked 
     * back in the original calling function the addr is actually
     * chosen... in a way, you could say this is similar to getaddrinfo()!
     */
    struct l3_binding *bind;
    
    printk(KERN_INFO "called translate_af_mhost\n");

    bind = binding_from_id(ma->id_no);
    if (bind) {
        return bind->l3_head;
    }
    
    return NULL;
}

static struct l3_binding * binding_from_id(short id)
{
    struct l3_binding *bind;
    for (bind = table_head; bind; bind = bind->next) {
        if (bind->id == id) {
            break;
        }
    }
    
    return bind;
}

static int mhost_translate_init(struct l3_binding *binding)
{
    table_head = binding;
    binding->next = NULL;
    return 0;
}

static int mhost_translate_insert(struct l3_binding *binding)
{
    struct l3_binding *ptr = table_head;
    struct l3_binding *nxt = NULL;
    
    if (table_head == NULL) {
        /* just in case list was uninitialized */
        return mhost_translate_init(binding);
    }
    
    /* case where we have a new list head */
    if (binding->id < table_head->id) {
        binding->next = table_head;
        table_head = binding;
        return 0;
    }
    
    do {
        nxt = ptr->next;
        
        /* insert proto between ptr and nxt 
         * this code also works for when nxt is NULL (EOL)
         */
        if (!nxt || binding->id < nxt->id) {
            if (binding->id == ptr->id) {
                /* ERROR: protocol already registered! */
                return -1;
            }
            ptr->next = binding;
            binding->next = nxt;
            return 0;
        }
        
        ptr = nxt;
    } while (1);
    
    /* should never get here? */
    return -1;
}

int insert_sockaddr_id(struct sockaddr *sa, short id)
{
    /* first, create l3_addr struct info */
    struct mhost_proto *mp;
    struct l3_addr *entry;
    struct l3_binding *binding;
    printk(KERN_INFO "called insert_sockaddr_id\n");
    
    mp = mhost_proto_for_family(sa->sa_family);
    if (!mp) {
        printk(KERN_INFO "error: mhost proto not supported!\n");
        return -1;
    }
    
    entry = kmalloc(sizeof(struct l3_addr), GFP_KERNEL);
    entry->mp = mp;
    memcpy(&(entry->addr), sa, sizeof(struct sockaddr));
    entry->next = NULL;
    
    /* now check for an l3 binding, create one if none exist */
    binding = binding_from_id(id);
    
    if (!binding) {
        /* create a suitable binding here! */
        printk(KERN_INFO "creating binding for id:%d\n", id);
        binding = kmalloc(sizeof(struct l3_binding), GFP_KERNEL);
        binding->id = id;
        binding->l3_head = NULL;
        mhost_translate_insert(binding);
    }
    
    return binding_insert_addr(binding, entry);
}

/* there's no real reason to organize this list at all
 * so we just add the address to the end of it! */
static int binding_insert_addr(struct l3_binding *binding, struct l3_addr *entry)
{
    struct l3_addr *ptr;
    ptr = binding->l3_head;
    
    /* case where no active bindings exist...
     * this should never happen, but just-in-case! */
    if (ptr == NULL) {
        binding->l3_head = entry;
        return 0;
    }
    
    /* move down the list to get to the end of it */
    while (ptr->next) {
        ptr = ptr->next;
    }
    ptr->next = entry;
    entry->next = NULL;
    return 0;
}