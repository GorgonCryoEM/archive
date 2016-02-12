void Volume::surfaceSkeleton(Volume* grayvol, float lowthr, float highthr) {
    int i, j, k;
    threshold(0.5f, -MAX_ERODE, 0);

    GridQueue* queue2 = new GridQueue();
    GridQueue* queue3 = new GridQueue();
    GridQueue* queue4 = new GridQueue();

    PriorityQueue < gridPoint > queue(
            MAX_QUEUELEN);
    int ct = 0;

    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if(getDataAt(i, j, k) >= 0) {
                    float v = (float)grayvol->getDataAt(i, j, k);
                    if(v > highthr || v <= lowthr) {
                        setDataAt(i, j, k, MAX_ERODE);
                    }
                    else {
                        ct++;

                        for(int m = 0; m < 6; m++) {
                            if(getDataAt(i + neighbor6[m][0],
                                       j + neighbor6[m][1], k + neighbor6[m][2])
                               < 0) {
                                queue2->prepend(i, j, k);
                                break;
                            }
                        }
                    }
                }
            }

    // Perform erosion
    int wid = MAX_ERODE;
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

        int numComplex = 0, numSimple = 0;
#ifdef VERBOSE
        printf("Processing %d nodes in layer %d\n", queue2->getNumElements(), curwid);
#endif

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

        queue2->reset();
        ele = queue2->getNext();
        while(ele != NULL) {
            ox = ele->x;
            oy = ele->y;
            oz = ele->z;

            // Compute score
            score = getNumPotComplex(ox, oy, oz);
            scrvol->setDataAt(ox, oy, oz, score);

            // Push to queue
            gridPoint gp(ox, oy, oz);



            // queue.add( gp, -score ) ;
            queue.add(gp, score);

            ele = queue2->remove();
        }

        delete queue2;
        queue2 = queue3;
        queue3 = new GridQueue();

        int nowComplex = 0;

        // Next, start priority queue iteration
        while(!queue.isEmpty()) {
            // Retrieve the node with the highest score
            queue.remove(gp, score);
            ox = gp.x;
            oy = gp.y;
            oz = gp.z;


            if(getDataAt(ox, oy, oz) != curwid || (int)scrvol->getDataAt(ox, oy,
                                                          oz)
                                                  != score) {
                continue;
            }

            if( (!isSimple(ox, oy, oz)) || isSheetEnd(ox, oy, oz)) {
                // Complex, set to next layer
                setDataAt(ox, oy, oz, curwid + 1);
                queue4->prepend(ox, oy, oz);
                numComplex++;

                nowComplex = 1;
            }
            else {
                setDataAt(ox, oy, oz, -1);
                numSimple++;

                for(int m = 0; m < 6; m++) {
                    int nx = ox + neighbor6[m][0];
                    int ny = oy + neighbor6[m][1];
                    int nz = oz + neighbor6[m][2];
                    if(getDataAt(nx, ny, nz) == 0) {
                        // setDataAt( nx, ny, nz, curwid + 1 ) ;
                        queue2->prepend(nx, ny, nz);
                    }
                }

                if(nowComplex) {

                    // printf("Error: %d\n", score);
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
                            score = getNumPotComplex(nx, ny, nz);

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

        if(numSimple == 0) {
            if(queue2->getNumElements() > 0) {
                printf(
                        "*************************wierd**********************\n");
            }
            break;
        }
    }

    // Remove all internal voxels (contained in cells)

    queue4->reset();
    ele = queue4->getNext();
    while(ele != NULL) {
        ele = queue4->remove();
    }

    queue2->reset();
    for(i = 0; i < getSizeX(); i++)
        for(j = 0; j < getSizeY(); j++)
            for(k = 0; k < getSizeZ(); k++) {
                if(getDataAt(i, j, k) == 0 && isInternal2(i, j, k) == 1) {
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
