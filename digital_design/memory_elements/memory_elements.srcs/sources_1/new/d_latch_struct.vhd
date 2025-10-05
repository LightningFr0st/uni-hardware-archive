----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 02:07:08 PM
-- Design Name: 
-- Module Name: d_latch_struct - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity d_latch_struct is
    Port ( D : in STD_LOGIC;
           E : in STD_LOGIC;
           Q : out STD_LOGIC;
           nQ : out STD_LOGIC);
end d_latch_struct;

architecture Structural of d_latch_struct is

component rs_latch_struct is
    Port ( S : in STD_LOGIC;
       R : in STD_LOGIC;
       Q : out STD_LOGIC;
       nQ : out STD_LOGIC);
end component;

component and2 is 
    Port(
    A : in std_logic;
    B : in std_logic;
    Q : out std_logic);
end component;

component inv is
    Port ( I : in STD_LOGIC;
           Q : out STD_LOGIC);
end component;

signal s_int, r_int, not_d : std_logic;

begin
    INV_1 : inv port map(
        I => D,
        Q => not_d
    );

    AND_1 : and2 port map(
        A => E,
        B => D,
        Q => s_int
    );       
    AND_2 : and2 port map(
        A => E,
        B => not_d,
        Q => r_int
    );
    RS : rs_latch_struct port map(
        S => s_int,
        R => r_int,
        Q => Q,
        nQ => nQ
    );
end Structural;
