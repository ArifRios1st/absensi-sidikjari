<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\Relations\BelongsTo;
use Illuminate\Database\Eloquent\Casts\Attribute;

class Fingerprint extends Model
{
    public $timestamps = false;
    public $incrementing = false;
    protected $keyType = 'int';

    protected $fillable = [
        'id',
        'id_guru',
        'scanned_at',
    ];

    protected $casts = [
        'scanned_at' => 'datetime:Y-m-d H:i:s',
        'is_scanned' => 'boolean',
    ];

    protected function isScanned(): Attribute
    {
        return Attribute::make(
            get: fn (mixed $value, array $attributes) => !is_null($attributes['scanned_at'] ?? null),
            set: fn (mixed $value) => ['scanned_at' => $value ? now() : null],
        );
    }

    public function guru() : BelongsTo
    {
        return $this->belongsTo(Guru::class, 'id_guru','id');
    }
}
