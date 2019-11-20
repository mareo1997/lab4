/*
 * elevator clook
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

char rW = 'R';

struct clook_data {
	struct list_head queue; //List of pending requests
};

static void clook_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
//request * Pointer to descriptor of the request in the queue to be considered 
//1st for possible merging
	list_del_init(&next->queuelist);//elt to del from list
}

static int clook_dispatch(struct request_queue *q, int force)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {//Dispatches 1 request unless empty
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		//get struct for this entry
		list_del_init(&rq->queuelist);
		//printk("[CLOOK] dsp [%c] [%lu]", rW, blk_rq_pos(rq)); 
		elv_dispatch_sort(q, rq);
		printk("[CLOOK] dsp [%c] [%lu]\n", rW, blk_rq_pos(rq));
		return 1;
	}
	return 0;
}

static void clook_add_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;
	struct list_head *head = NULL; //use as the initial head position
	
	//elv_add_request(&rq->queuelist, &nd->queue);
	list_for_each(head, &nd->queue){
		if(rq_end_sector(list_entry(head, struct request, queuelist)) > rq_end_sector(rq)){
			break;
		}
	}
	list_add_tail(&rq->queuelist, head);//Add request to the end
	printk("[CLOOK] add [%c] [%lu]\n", rW, blk_rq_pos(rq));
}

static struct request *
clook_former_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
clook_latter_request(struct request_queue *q, struct request *rq)
{
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int clook_init_queue(struct request_queue *q)
{
	struct clook_data *nd;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return -ENOMEM;

	INIT_LIST_HEAD(&nd->queue);
	q->elevator->elevator_data = nd;
	return 0;
}

static void clook_exit_queue(struct elevator_queue *e)
{
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void){return elv_register(&elevator_clook);}

static void __exit clook_exit(void){elv_unregister(&elevator_clook);}

//static void clook_add(void, void){elv_add_request(&elevator_clook);}

//static int clook_dsp(void, int){elv_drain_elevator(&elevator_clook);}

module_init(clook_init);
//module_add(clook_add);
module_exit(clook_exit);


MODULE_AUTHOR("Mareo Yapp");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Clook IO scheduler");
