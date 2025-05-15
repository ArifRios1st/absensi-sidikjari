<?php

namespace App\Models;

use App\Casts\Time;
use App\Enums\HariEnum;
use Carbon\Carbon;
use Illuminate\Database\Eloquent\Casts\Attribute;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\Relations\BelongsTo;

class Absensi extends Model
{
    protected $table = "absensi";

    protected $fillable = [
        'id_guru',
        'tanggal',
        'jam_masuk',
        'jam_keluar',
        'jadwal_masuk',
        'jadwal_keluar',
        'toleransi_keterlambatan'
    ];

    protected $casts = [
        'tanggal' => 'date:d-m-Y',
        'jam_masuk' => Time::class,
        'jam_keluar' => Time::class,
        'jadwal_masuk' => Time::class,
        'jadwal_keluar' => Time::class,
        'is_late' => 'boolean'
    ];

    protected function isLate(): Attribute
    {
        return Attribute::make(
            get: function ($value, array $attributes) {
                // Ensure required values are not null
                if (empty($attributes['jam_masuk']) || empty($attributes['jadwal_masuk']) || !isset($attributes['toleransi_keterlambatan'])) {
                    return false; // If missing, assume not late
                }

                $jamMasuk = Carbon::parse($attributes['jam_masuk']);
                $jadwalMasuk = Carbon::parse($attributes['jadwal_masuk'])->addMinutes($attributes['toleransi_keterlambatan']);

                return $jamMasuk->greaterThan($jadwalMasuk);
            }
        );
    }

    public function guru() : BelongsTo
    {
        return $this->belongsTo(Guru::class, 'id_guru','id');
    }
}
