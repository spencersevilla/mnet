#include "mhost_table.h"

static int mhost_table_af_specified(struct sockaddr *sa, struct sock *sk);
struct sockaddr * mhost_table_lookup(struct sockaddr_mhost *sa);

static struct l3_addr * translate_af_mhost(struct sockaddr_mhost *ma);
static struct l3_binding * binding_from_id(short id);
static struct sockaddr * addr_from_binding(struct l3_binding *binding);
static int mhost_translate_init(struct l3_binding *binding);
static int mhost_translate_insert(struct l3_binding *binding);
static int binding_insert_addr(struct l3_binding *binding, struct l3_addr *entry);

static struct mhost_proto *head;

/* internal structs for table lookups */
struct l3_addr {
    struct l3_addr *next;   /* linked-list */
    struct mhost_proto *mp; /* has short family as first member! */
    struct sockaddr *addr;  /* l3-specific valid address */
};

struct l3_binding {
    short id;                   /* needed until we do getaddrinfo */
    struct l3_binding *next;    /* linked-list */
    struct l3_addr *l3_head;    /* head of l3 address list */
};

static struct l3_binding *table_head = NULL;

/* entry function */
int mhost_translate_sa(struct sockaddr *sa, struct sock *sk)
{
    printk(KERN_INFO "called mhost_translate_sa\n");
    
    /* here we're given an AF_MHOST pointer so we translate it first */
    if (sa->sa_family == AF_MHOST) {
        struct sockaddr *new_sa = mhost_table_lookup((struct sockaddr_mhost *) sa);
        memcpy(sa, new_sa, sizeof(struct sockaddr));
    }
    
    /* error-check the sa before we work with it... */
    if (sa->sa_family == AF_MHOST || sa->sa_family == AF_UNSPEC) {
        printk(KERN_INFO "error: mhost_translate_sa given a bad sockaddr\n");
        return -EAFNOSUPPORT;
    }
    
    /* this last call sets the proto function pointer */
    return mhost_table_af_specified(sa, sk);
};

struct sockaddr * mhost_table_lookup(struct sockaddr_mhost *sa) {
    /*  Kevin:
     *  Currently, this function is completely empty... it's your main
     *  entry-point for the table. your input is a sockaddr_mhost structure
     *  which has fields for sa_family (set to AF_MHOST), port, and an id number.
     *  The id number should be demuxed, do whatever you want with it,
     *  and return a struct sockaddr containing the appropriate address.
     */
	//Added by KABAS (12-10-2012)
        
        //First we demux the sockaddr_mhost structure to its l3_addr list
	struct l3_addr *first = translate_af_mhost(sa);
        
        /*
	Since we're currently pointing to the first l3_addr structure
        we then just pass the sockaddr it contains. In the future smarts
        will be implemented here to correctly select a working l3_addr 
	*/
	struct sockaddr *answer = first->addr; 
	
	printk(KERN_INFO "Called mhost_table_lookup\n"); 
        return answer;
};

// Added by MSEVILLA (12-10-2012)
struct sockaddr * mhost_get_l3_head(int id) {
	struct l3_binding *current_binding;
	struct sockaddr_in *casted_ret;
	struct sockaddr *ret;
	current_binding = binding_from_id(id);

	ret = addr_from_binding(current_binding);
	casted_ret = (struct sockaddr_in *) ret;
	printk(KERN_INFO "\n mhost_get_l3_head head port: %d\n", ntohs(casted_ret->sin_port));
	printk(KERN_INFO "mhost_get_l3_head head addr: %d\n", htonl(casted_ret->sin_addr.s_addr));
	//return addr_from_binding(current_binding);
	return ret;
}

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
    printk(KERN_INFO "mhost_table_af_specified \n");

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
    //printk(KERN_INFO "mhost_proto_for_family \n");

    while (answer) {
        if (family == answer->family) {
            break;
        }
        
        /* went past it, so we don't have it */
        if (family < answer->family) {
            answer = NULL;
            printk(KERN_INFO "error: mhost_proto_for_family \n");
            break;
        }
        
        /* no more protocols registered */
        if (answer->next == NULL) {
            answer = NULL;
            printk(KERN_INFO "error: mhost_proto_for_family \n");
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

int mhost_table_unregister(int family) {
    struct mhost_proto *ptr;
    struct mhost_proto *nxt;

    printk(KERN_INFO "mhost_proto_for_family removing family %d\n", family);

    /* special-case where the head is the answer */
    if (family == head->family) {
        head = head->next;
    }

    /* ptr stays one step behind for next-ing */
    ptr = head;
    nxt = head->next;
    while (nxt != NULL) {
        if (family == nxt->family) {
            ptr->next = nxt->next;
            return 0;
        }

        /* went past it, so we don't have it */
        if (family < nxt->family) {
            return -1;
        }

        ptr = nxt;
        nxt = nxt->next;
    }

    /* nxt = NULL means we went through the entire list! */
    return -1;
}

/* MIKE: The following functions might be useful to you. I don't think they're hooked-up to anything,
 * nor do I remember exactly what I was doing with them, but I'm pretty sure they were meant to work with
 * the table. Take a look at the structs l3_binding and l3_addr... I think I was using them to make a linked
 * list? Again, not 100% what I was doing here but they might help you out. 
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

// Added by MSEVILLA (12-10-2012)
static struct sockaddr * addr_from_binding(struct l3_binding *binding)
{
	struct l3_addr *address;
	//struct sockaddr_in *ret;
	printk(KERN_INFO "Printing out %d's addresses\n", binding->id);
	for (address = binding->l3_head; address; address = address->next) {
	    struct sockaddr_in *casted = (struct sockaddr_in *) &(address->addr);
	    printk(KERN_INFO "\tport: %d\n", ntohs(casted->sin_port));
	    printk(KERN_INFO "\taddr: %d\n", htonl(casted->sin_addr.s_addr));
	}
	
	//ret = (struct sockaddr_in *) &(binding->l3_head->addr);
	//printk(KERN_INFO "\taddr_from_binding head port: %d\n", ntohs(ret->sin_port));
	//printk(KERN_INFO "\thead addr: %d\n", htonl(ret->sin_addr.s_addr));
	
	return (struct sockaddr *) &(binding->l3_head->addr);
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
