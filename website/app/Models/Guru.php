<?php

namespace App\Models;

use App\Enums\GenderEnum;
use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\Relations\HasMany;

class Guru extends Model
{
    use HasFactory;

    protected $table = "guru";

    protected $fillable = [
        'nip',
        'nama',
        'jenis_kelamin',
        'no_hp',
        'alamat',
    ];

    protected $casts = [
        'jenis_kelamin' => GenderEnum::class,
    ];

    public function dataAbsensi() : HasMany
    {
        return $this->hasMany(Absensi::class,'id_guru','id');
    }

    public function fingerprints() : HasMany
    {
        return $this->hasMany(Fingerprint::class,'id_guru','id');
    }


}
