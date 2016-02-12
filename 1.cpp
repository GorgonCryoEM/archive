void Volume::curveSkeleton(Volume* grayvol, float lowthr, float highthr,
                           Volume* svol)
{
    int i, j, k;
    // First, threshold the volume
#ifdef VERBOSE
    printf("Thresholding the volume to -1/0...\n");
#endif
    threshold(0.5f, -1, 0);

    // Next, apply convergent erosion
    // by preserving: complex nodes, curve end-points, and sheet points

    // Next, initialize the linked queue
#ifdef VERBOSE
    printf("Initializing queue...\n");
#endif
    GridQueue* queue2 = new GridQueue();
    GridQueue* queue3 = new GridQueue();
    GridQueue* queue4 = new GridQueue();
    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if(getDataAt(i, j, k) >= 0) {
                    double v = grayvol->getDataAt(i, j, k);
                    if(svol->getDataAt(i, j, k) > 0
                       || v <= lowthr || v > highthr) {
                        setDataAt(i, j, k, MAX_ERODE);
                    }
                    else {
                        for(int m = 0; m < 6; m++) {
                            if(getDataAt(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                // setDataAt( i, j, k, 1 ) ;
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }
    int wid = MAX_ERODE;
#ifdef VERBOSE
    printf("Total %d nodes\n", queue2->getNumElements() );
    printf("Start erosion to %d...\n", wid);
#endif

    // Perform erosion
    gridQueueEle* ele;
    gridPoint gp;
    int ox, oy, oz;
    int score;
    Volume* scrvol = new Volume(this->getSizeX(), this->getSizeY(),
            this->getSizeZ());
    for(i = 0; i < getSizeX() * getSizeY() * getSizeZ(); i++) {
        scrvol->setDataAt(i, -1);
    }

    for(int curwid = 1; curwid <= wid; curwid++) {
        // At the start of each iteration,
        // queue2 holds all the nodes for this layer
        // queue3 and queue are empty

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid);
#endif

        /*
         We first need to assign curwid + 1 to every node in this layer
         */
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            if(getDataAt(ox, oy, oz) == curwid) {
                ele = queue2->remove();
            }
            else {
                setDataAt(ox, oy, oz, curwid);
                ele = queue2->getNext();
            }
        }
        queue4->reset();
        ele = queue4->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            queue2->prepend(ox, oy, oz);
            ele = queue4->remove();
        }

        // Next,
        // Compute score for each node left in queue2
        // move them into priority queue
        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex2(ox, oy, oz);
            scrvol->setDataAt(ox, oy, oz, score);

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        // Rename queue3 to be queue2,
        // Clear queue3
        // From now on, queue2 holds nodes of next level
        delete queue2;
        queue2 = queue3;
        queue3 = new GridQueue();

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;

//				score = -score ;

            // Ignore the node
            // if it has been processed before
            // or it has an updated score
            if(getDataAt(ox, oy, oz) != curwid || (int)scrvol->getDataAt(ox, oy,
                                                          oz)
                                                  != score) {
                continue;
            }

            if(isHelixEnd(ox, oy, oz) || !isSimple(ox, oy, oz)) {
                // Complex, set to next layer
                setDataAt(ox, oy, oz, curwid + 1);
                queue4->prepend(ox, oy, oz);
                numComplex++;
            }
            else {
                setDataAt(ox, oy, oz, -1);
                numSimple++;

                /* Adding ends */
                // Move its neighboring unvisited node to queue2
                for(int m = 0; m < 6; m++) {
                    int nx = ox + neighbor6[m][0];
                    int ny = oy + neighbor6[m][1];
                    int nz = oz + neighbor6[m][2];
                    if(getDataAt(nx, ny, nz) == 0) {
                        // setDataAt( nx, ny, nz, curwid + 1 ) ;
                        queue2->prepend(nx, ny, nz);
                    }
                }

            }

            // Update scores for nodes in its 5x5 neighborhood
            // insert them back into priority queue
            for(i = -2; i < 3; i++)
                for(j = -2; j < 3; j++)
                    for(k = -2; k < 3; k++) {
                        int nx = ox + i;
                        int ny = oy + j;
                        int nz = oz + k;

                        if(getDataAt(nx, ny, nz) == curwid) {
                            // Compute score
                            score = getNumPotComplex2(nx, ny, nz);

                            if(score != (int)scrvol->getDataAt(nx, ny, nz)) {
                                // printf("Update\n") ;
                                scrvol->setDataAt(nx, ny, nz, score);
                                // Push to queue
                                gridPoint gp(ox, oy, oz);
                                gp.x = nx;
                                gp.y = ny;
                                gp.z = nz;
                                // queue.add( gp, -score ) ;
                                queue.add(gp, score);
                            }
                        }
                    }

        }

#ifdef VERBOSE
        printf("%d complex, %d simple\n", numComplex, numSimple);
#endif

        if(numSimple == 0) {
            break;
        }
    }

    // Remove all internal voxels (contained in manifold surfaces)
    queue2->reset();
    queue4->reset();
    ele = queue4->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;

        if(isPiercable(ox, oy, oz) == 1) // hasCompleteSheet( ox, oy, oz ) == 1 ) //
        {
            queue2->prepend(ox, oy, oz);
            //	setDataAt( ox, oy, oz, -1 ) ;
        }
        ele = queue4->remove();
    }

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if(getDataAt(i, j, k) == 0 && isPiercable(i, j, k)) //hasCompleteSheet(i,j,k) == 1) //
                           {
                    queue2->prepend(i, j, k);
                }
            }
    queue2->reset();
    ele = queue2->getNext();
    while(ele != NULL) {
        ox = ele->x;
        oy = ele->y;
        oz = ele->z;
        setDataAt(ox, oy, oz, -1);
        ele = queue2->remove();
    }

    // Finally, clean up
    delete scrvol;

    delete queue2;
    delete queue3;
    delete queue4;
#ifdef VERBOSE
    printf("Thresholding the volume to 0/1...\n");
#endif
    threshold(0, 0, 1);
}
