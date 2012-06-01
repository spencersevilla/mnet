#include "mhost_table.h"
#include "mhost_inet.h"

static int mhost_table_af_specified(struct sockaddr *sa, struct sock *sk);

static struct mhost_proto *head;

/* entry function */
int mhost_table_lookup(struct sockaddr *sa, struct sock *sk)
{
//    struct mhost_sock *ms = mhost_sk(sk);
    
    /* check if an AF (such as AF_INET) is specified explicitly */
    if (sa->sa_family != AF_MHOST && sa->sa_family != AF_UNSPEC) {
        return mhost_table_af_specified(sa, sk);
    }
    
    /* here we're given an AF_MHOST addressing structure
     * so we must translate the addressing information
     * in addition to finding the right handler!
     */
    
    /* not implemented yet, so just return an error */
    return -1;
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