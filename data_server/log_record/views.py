from django.shortcuts import render


from models import * ;
import traceback,json;
from prettytable import PrettyTable ;
from django.http import Http404,HttpResponse ;

def add_orignal_log(req):
    try:
        inp = req.read();
        c_log = ecs_orignal_log(log=inp);
        c_log.save();
    except:
        print traceback.format_exc();
        raise Http404();
    return HttpResponse(json.dumps({"res":"ok"}));

def list_orignal_log_table(req):
    try:
        x = PrettyTable(["utime","log"]);
        x.align["utime"]="l";
        x.align["log"]="l";
        f_logs = ecs_orignal_log.objects.all().order_by("utime")[:100];
        for f in f_logs:
            x.add_row([f.utime.ctime(),str(f.log)])
    except expression as identifier:
        print traceback.format_exc();
        raise Http404();
    return HttpResponse("<html><body><pre>"+str(x)+"</pre></body></html>");