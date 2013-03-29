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
        print "opening pickle file\n"
        data_file = open('metadata.ro.pickle','rb')
        data = pickle.load(data_file)
        data_file.close()
    except:
        sys.exit()
    print "got data: ",len(data)," records\n"
    file = open('unique_ids',"r")
    ids = [x.rstrip() for x in file.readlines()]
    file.close()
    nvids = len(ids)
    file = open('metadata.csv',"w")
    print >>file,"id,title,description,date,nraters,avg_rating,nviews,nfavorites"
    not_found = 0
    
    for i in range(0,nvids):
        id = ids[i]
        # now output to csv file
        d = data[id]
        if d == None:
            not_found += 1
            continue # skip missing videos
        out_str = id + ","
        if d.media.title.text != None:
            out_str += (d.media.title.text).replace(","," ")
        out_str += ","
        if d.media.description.text != None:
            out_str += (d.media.description.text).replace(","," ")
        out_str += ","
        if d.published.text != None:
            out_str += d.published.text
        out_str += ","
        if(d.rating != None):
            out_str += str(d.rating.num_raters)+","+ str(d.rating.average)+","
        else:
            out_str += ",,"
        out_str += str(d.statistics.view_count)+","+str(d.statistics.favorite_count)
        out_str = out_str.replace("\n"," ")
        print >>file, out_str
    
    print not_found," videos were not found"
if __name__ == "__main__":
    main()

