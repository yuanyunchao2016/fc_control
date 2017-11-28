from django.test import TestCase
from models import * ;
import traceback,Http404,HttpResponse ;
# Create your tests here.

def add_orignal_log(req):
    try:
        inp = req.read();
        c_log = ecs_orignal_log(log=inp);
        c.save();
    except:
        print traceback.format_exc();
        raise Http404();
    return HttpResponse(json.dumps({"res":"ok"}));

