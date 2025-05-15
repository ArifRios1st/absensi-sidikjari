<?php

namespace App\Filament\App\Resources;

use App\Enums\HariEnum;
use App\Filament\App\Resources\JadwalResource\Pages;
use App\Models\Jadwal;
use BezhanSalleh\FilamentShield\Contracts\HasShieldPermissions;
use Filament\Forms;
use Filament\Forms\Form;
use Filament\Resources\Resource;
use Filament\Tables;
use Filament\Tables\Table;

class JadwalResource extends Resource implements HasShieldPermissions
{
    protected static ?string $model = Jadwal::class;
    protected static ?string $navigationGroup = 'Data Absensi';
    protected static ?string $navigationIcon = 'heroicon-o-clock';
    protected static ?string $slug = 'jadwal';

    public static function form(Form $form): Form
    {
        return $form
            ->schema([
                Forms\Components\Select::make('hari')
                    ->label('Hari')
                    ->validationAttribute('Hari')
                    ->options(HariEnum::class)
                    ->enum(HariEnum::class)
                    ->searchable()
                    ->required()
                    ->unique('jadwal',ignoreRecord: true)
                    ->columnSpanFull(),
                Forms\Components\TimePicker::make('jam_masuk')
                    ->label('Jam Masuk')
                    ->validationAttribute('Jam Masuk')
                    ->seconds(false)
                    ->required(),
                Forms\Components\TimePicker::make('jam_keluar')
                    ->label('Jam Keluar')
                    ->validationAttribute('Jam Keluar')
                    ->seconds(false)
                    ->required(),
                Forms\Components\TextInput::make('toleransi_keterlambatan')
                    ->numeric()
                    ->label('Toleransi (Menit)')
                    ->validationAttribute('Toleransi')
                    ->required()
                    ->columnSpanFull(),
            ]);
    }

    public static function table(Table $table): Table
    {
        return $table
            ->columns([
                Tables\Columns\TextColumn::make('hari')
                    ->sortable()
                    ->badge()
                    ->label('Hari'),
                Tables\Columns\TextColumn::make('jam_masuk')
                    ->sortable()
                    ->label('Jam Masuk'),
                Tables\Columns\TextColumn::make('jam_keluar')
                    ->sortable()
                    ->label('Jam Keluar'),
                Tables\Columns\TextColumn::make('toleransi_keterlambatan')
                    ->sortable()
                    ->label('Toleransi (Menit)'),
            ])
            ->filters([
                //
            ])
            ->actions([
                Tables\Actions\ViewAction::make(),
                Tables\Actions\EditAction::make(),
                Tables\Actions\DeleteAction::make(),
            ])
            ->bulkActions([
                Tables\Actions\BulkActionGroup::make([
                    Tables\Actions\DeleteBulkAction::make(),
                ]),
            ]);
    }

    public static function getRelations(): array
    {
        return [
            //
        ];
    }

    public static function getPages(): array
    {
        return [
            'index' => Pages\ListJadwals::route('/'),
            'create' => Pages\CreateJadwal::route('/create'),
            'edit' => Pages\EditJadwal::route('/{record}/edit'),
        ];
    }

    public static function getPermissionPrefixes(): array
    {
        return [
            'view',
            'view_any',
            'create',
            'update',
            'delete',
            'delete_any',
        ];
    }
}
