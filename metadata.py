#!/usr/bin/env python
import os
import sys
import pickle
import gdata.youtube
import gdata.youtube.service

def main():
#    logger = logging.getLogger()
#    logger.setLevel(logging.DEBUG)
    yt_service = gdata.youtube.service.YouTubeService()
    old_file_exists = True
    try:
        data_file = open('metadata.pickle','rb')
        data = pickle.load(data_file)
        data_file.close()
    except:
        data = dict()
        old_file_exists = False
    file = open('unique_ids',"r")
    ids = [x.rstrip() for x in file.readlines()]
    nf = 0
    file.close()

    nvids = 10 #len(ids)
    
    for i in range(0,nvids):
        id = ids[i]
        if id in data:
            #print id," exists"
            continue
        too_fast = True
        while(too_fast):
            too_fast = False
            try:
                data[id] = yt_service.GetYouTubeVideoEntry(video_id=id)
            except gdata.service.RequestError, err:
                response = err[0]
#                print response['body']
                if "too_many" in response['body']:
                    too_fast = True
                    sys.stderr.write("sleeping 10 at %d\n" % i)
                    os.system("sleep 10")
                else:
                    data[id] = None
                    nf += 1
    
        d = data[id]
        print "\nvideo id: " + ids[i]# + "data: ", d
# "statistics: " +  #str(d.statistics).media.title.text
        if d == None:
            print "not found"
        else:
            print "title: %s" % d.media.title.text
            if(d.rating != None):
                print "no. raters: " + str(d.rating.num_raters)
                print "avg. rating: " + str(d.rating.average)
            print "view count: " + str(d.statistics.view_count)
            print "favorite count: " + str(d.statistics.favorite_count)
            if d.comments != None:
                print "comments: %s" % d.comments.text
            print "tags: %s" % d.media.keywords.text
        if i > 0 and i%1000 == 0: # checkpoint
            data_file = open('metadata.pickle','wb')
            pickle.dump(data,data_file)
            data_file.close() 
            
    print nf," videos were not found"
    data_file = open('metadata.pickle','wb')
    pickle.dump(data,data_file)
    data_file.close()

if __name__ == "__main__":
    main()

