<?php

namespace App\Models;

use App\Casts\Time;
use App\Enums\HariEnum;
use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Jadwal extends Model
{
    use HasFactory;

    protected $table = "jadwal";

    protected $fillable = [
        'hari',
        'jam_masuk',
        'jam_keluar',
        'toleransi_keterlambatan',
    ];

    protected $casts = [
        'hari' => HariEnum::class,
        'jam_masuk' => Time::class,
        'jam_keluar' => Time::class,
    ];
}
