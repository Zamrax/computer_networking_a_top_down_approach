#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "prog3.h"
#include "node3.h"

#define NODE_ID 3

int connectcosts3[4] = { 7,  999,  2, 0 };

struct distance_table dt3;


/* students to write the following two routines, and maybe some others */


void rtinit3() {
    for (int i = 0; i < 4; i++) {
        dt3.costs[i][i] = connectcosts3[i];
    }

    struct rtpkt cost_pkt;
    cost_pkt.sourceid = NODE_ID;
    cost_pkt.destid = 0;
    memcpy(&cost_pkt.mincost, connectcosts3, 4 * sizeof(int));

    struct rtpkt *cost_pkt_ptr = &cost_pkt;

    // ignore self (link cost 0) and non directly connected nodes (999)
    // send direct neighbours our link costs
    for (int i = 0; i < 4; i++) {
        if (connectcosts3[i] != 0 && connectcosts3[i] != 999) {
            cost_pkt_ptr->destid = i;
            tolayer2(cost_pkt);
        }
    }

    printdt3(&dt3);
}


void rtupdate3(struct rtpkt *rcvdpkt) {
    printf("----------\n");
    printf("rtupdate3 srcid: %i\n", rcvdpkt->sourceid);
    printf("rtupdate3 destid: %i\n", rcvdpkt->destid);
    printf("rtupdate3 mincosts: ");
    for (int i = 0; i < 4; i++) {
        printf("%i ", rcvdpkt->mincost[i]);
    }
    printf("\n");

    // Steps to take:
    // - Check srcid (use as index for 2d distance table)
    // - Iterate through mincosts array (index is destination node)
    // - If connectcosts3[sourceid] + rcvdpkt->mincosts[i] < connectcosts3[sourceid] + dt3.costs[sourceid][i] then update dt3.costs[sourceid][i]
    // - If update made in last step, then resend dt3 to directly connected nodes

    bool table_updated = false;

    printf("----------\n");

    for (int i = 0; i < 4; i++) {
        // Don't update the distance table with another node's link cost to itself (which will always be 0)
        if (i == rcvdpkt->sourceid) {
            printf("i %i matches sourceid %i. Skipping.\n", i, rcvdpkt->sourceid);
            continue;
        }

        // Don't update the distance table with another node's link cost back to the current node (which we already have from mincosts[])
        if (i == NODE_ID) {
            printf("i %i matches NODE_ID %i. Skipping.\n", i, NODE_ID);
            continue;
        }

        // Ignore link costs of 999, which mean a node has not yet established a path to another node.
        if (rcvdpkt->mincost[i] == 999) {
            printf("node %i has not yet established a path to node %i. Skipping.\n", rcvdpkt->sourceid, i);
            continue;
        }

        printf("dt3.costs[i][srcid]: %i\n", dt3.costs[i][rcvdpkt->sourceid]);
        printf("connectcosts3[srcid]: %i\n", connectcosts3[rcvdpkt->sourceid]);
        printf("connectcosts3[srcid] + rcvdpkt->mincost[i]: %i\n", connectcosts3[rcvdpkt->sourceid] + rcvdpkt->mincost[i]);
        if (dt3.costs[i][rcvdpkt->sourceid] == 0 || (connectcosts3[rcvdpkt->sourceid] + rcvdpkt->mincost[i]) < dt3.costs[i][rcvdpkt->sourceid]) {
            dt3.costs[i][rcvdpkt->sourceid] = connectcosts3[rcvdpkt->sourceid] + rcvdpkt->mincost[i];
            table_updated = true;
        }
    }

    printf("----------\n");

    if (table_updated == true) {
        printf("dt3 was updated. New table below.\n\n");
        printdt3(&dt3);
    }
}


void printdt3(struct distance_table *dtptr) {
    printf("------------------------\n");
    printf("             via        \n");
    printf("   D3 |    0     2      \n");
    printf("------|-----------------\n");
    printf("     0|  %3d   %3d      \n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 1|  %3d   %3d      \n", dtptr->costs[1][0], dtptr->costs[1][2]);
    printf("     2|  %3d   %3d      \n", dtptr->costs[2][0], dtptr->costs[2][2]);
    printf("------------------------\n");
}
