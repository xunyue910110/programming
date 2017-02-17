from django.db import models

# Create your models here.

class Product(models.Model):
	name = models.CharField('product name', max_length=30)
	price = models.FloatField('price', default=10)
