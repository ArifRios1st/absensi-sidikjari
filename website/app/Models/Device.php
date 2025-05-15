<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;
use Illuminate\Support\Facades\Cache;

class Device extends Model
{
    public $timestamps = false;

    protected $table = "device";

    protected $fillable = [
        'token',
        'kapasitas',
    ];

    public static function kapasitasDevice() : int
    {
        return Cache::remember('kapasitasDevice', now()->addMinutes(10), function () {
            return Device::select('kapasitas')->first()->kapasitas;
        });
    }
}
