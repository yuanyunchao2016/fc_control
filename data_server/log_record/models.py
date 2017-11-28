from django.db import models

# Create your models here.

class ecs_orignal_log(models.Model):
    utime = models.DateTimeField(auto_now=True);
    log = models.CharField(max_length=2048);


