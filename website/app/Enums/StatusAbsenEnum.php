<?php

namespace App\Enums;

use App\Traits\EnumTrait;

enum StatusAbsenEnum: string
{
    use EnumTrait;

    case HADIR = 'HADIR';
    case TELAT = 'TELAT';
    case ABSEN = 'ABSEN';
}
